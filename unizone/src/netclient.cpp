#include "netclient.h"
#include "downloadimpl.h"
#include "global.h"
#include "debugimpl.h"
#include "version.h"
#include "settings.h"
#include "platform.h"			// <postmaster@raasu.org> 20021114
#include "wstring.h"

#include <qapplication.h>
#include "regex/PathMatcher.h"
#include "util/TimeUtilityFunctions.h"
#include "reflector/RateLimitSessionIOPolicy.h"
#include "iogateway/MessageIOGateway.h"
#include "zlib/ZLibUtilityFunctions.h"

NetClient::NetClient(QObject * owner)
{
	setName( "NetClient" );

	fPort = 0;
	fServerPort = 0;
	fOwner = owner;
	fServer = QString::null;
	fOldID = QString::null;
	fSessionID = QString::null;
	fUserName = QString::null;
	fChannelLock.lock();
	fChannels = new Message();
	fChannelLock.unlock();
}

NetClient::~NetClient()
{
	fChannelLock.lock();
	if (fChannels)
	{
		delete fChannels;
	}
	fChannelLock.unlock();
	Disconnect();
	WaitForInternalThreadToExit();
}

// <postmaster@raasu.org> -- Add support for port numbers
status_t
NetClient::Connect(QString server)
{
	uint16 uiPort = 2960;
	int pColon = server.find(":");
	if (pColon >= 0)
	{
		uiPort = server.mid(pColon+1).toUShort();
		server = server.left(pColon);
	}
	return Connect(server,uiPort);
}

status_t
NetClient::Connect(QString server, uint16 port)
{
	Disconnect();

	PRINT("Starting thread\n");
	if (StartInternalThread() != B_NO_ERROR)
	{
		return B_ERROR;
	}
	PRINT("Internal thread running\n");

	PRINT("Adding new session\n");

	if (gWin->fSettings->GetChatLimit() == WSettings::LimitNone)
	{
		if (AddNewConnectSession((const char *) server.utf8(), port) != B_NO_ERROR)
		{
			return B_ERROR;
		}
	}
	else
	{
		AbstractReflectSessionRef ref(new ThreadWorkerSession(), NULL);
		ref()->SetGateway(AbstractMessageIOGatewayRef(new MessageIOGateway(), NULL));
		ref()->SetOutputPolicy(PolicyRef(new RateLimitSessionIOPolicy(WSettings::ConvertToBytes(
								gWin->fSettings->GetChatLimit())), NULL));
		ref()->SetInputPolicy(PolicyRef(new RateLimitSessionIOPolicy(WSettings::ConvertToBytes(
								gWin->fSettings->GetChatLimit())), NULL));
		if (AddNewConnectSession((const char *) server.utf8(), port, ref) != B_NO_ERROR)
			return B_ERROR;
	}

	PRINT("New session added\n");
	fServer = server;
	fServerPort = port;
	return B_NO_ERROR;
}

void
NetClient::Disconnect()
{
	PRINT("DISCONNECT\n");
	gWin->setCaption("Unizone");
	if (IsInternalThreadRunning()) 
	{
		ShutdownInternalThread();
		Reset(); 
		emit DisconnectedFromServer();
		PRINT("DELETING\n");
		WUserIter it = fUsers.begin();
		while (it != fUsers.end())
		{
			(*it).second()->RemoveFromListView();
			fUsers.erase(it);
			it = fUsers.begin();
		}
		PRINT("DONE\n");
	}
}

void
NetClient::SignalOwner()
{
	QCustomEvent *e = new QCustomEvent(NetClient::SignalEvent);
	if (e)
		QThread::postEvent(fOwner, e);
}

QString
NetClient::LocalSessionID() const
{
	return fSessionID;
}

void
NetClient::AddSubscription(QString str, bool q)
{
	MessageRef ref(GetMessageFromPool(PR_COMMAND_SETPARAMETERS));
	if (ref())
	{
		ref()->AddBool((const char *) str.utf8(), true);		// true doesn't mean anything
		if (q)
			ref()->AddBool(PR_NAME_SUBSCRIBE_QUIETLY, true);		// no initial response
		SendMessageToSessions(ref);
	}
}

void
NetClient::RemoveSubscription(QString str)
{
	MessageRef ref(GetMessageFromPool(PR_COMMAND_REMOVEPARAMETERS));
	if (ref())
	{
		ref()->AddString(PR_NAME_KEYS, (const char *) str.utf8());
		SendMessageToSessions(ref);
	}
}

bool
NetClient::ExistUser(QString sessionID)
{
	QString id = sessionID;
	if (fUsers.find(id) != fUsers.end())
		return true;
	return false;
}

WUserRef
NetClient::FindUser(QString sessionID)
{
	QString id = sessionID;
	WUserIter iter = fUsers.find(id);
	if (iter != fUsers.end())
		return (*iter).second;
	return WUserRef(NULL, NULL);
}

void 
NetClient::FindUsersByIP(WUserMap & umap, QString ip)
{
	for (WUserIter iter = fUsers.begin(); iter != fUsers.end(); iter++)
	{
		if ((*iter).second()->GetUserHostName() == ip)
		{
			WUserPair p = MakePair((*iter).second()->GetUserID(), (*iter).second);
			umap.insert(p);
			//return (*iter).second;
		}
	}
	return;
	// return WUserRef(NULL, NULL);
}

// will insert into list if successful
WUserRef
NetClient::CreateUser(QString sessionID)
{
	WUser * n = new WUser(sessionID);
	if (n)
	{
		n->SetUserID(sessionID);
		WUserRef nref(n, NULL);
		WUserPair pair = MakePair(sessionID, nref);

		fUsers.insert(pair);
		emit UserConnected(pair.first);
	}
	return WUserRef(n, NULL);	// NULL, or a valid user
}

void
NetClient::RemoveUser(QString sessionID)
{
	WUserIter iter = fUsers.find(sessionID);
	if (iter != fUsers.end())
	{
		emit UserDisconnected((*iter).first, (*iter).second()->GetUserName());
		PRINT("NetClient::RemoveUser: Removing from list\n");
		(*iter).second()->RemoveFromListView();
		PRINT("NetClient::RemoveUser: Erasing\n");
		fUsers.erase(iter);
		PRINT("NetClient::RemoveUser: Done\n");
	}
}

void
NetClient::HandleBeRemoveMessage(String nodePath)
{
	int pd = GetPathDepth(nodePath.Cstr());
	if (pd >= USER_NAME_DEPTH)
	{
		QString sid = QString::fromUtf8(GetPathClause(SESSION_ID_DEPTH, nodePath.Cstr()));
		sid = sid.mid(0, sid.find('/') );
		
		switch (pd)
		{
		case USER_NAME_DEPTH:
			{
				if (!strncmp(GetPathClause(USER_NAME_DEPTH, nodePath.Cstr()), "name", 4))
				{
					// user removed
					RemoveUser(sid);
				}
				break;
			}
			
		case FILE_INFO_DEPTH: 
			{
				const char * fileName = GetPathClause(FILE_INFO_DEPTH, nodePath.Cstr());
				emit RemoveFile(sid, fileName);
				break;
			}
		}
	}
}

void
NetClient::HandleUniRemoveMessage(String nodePath)
{
	int pd = GetPathDepth(nodePath.Cstr());
	if (pd >= USER_NAME_DEPTH)
	{
		QString sid = QString::fromUtf8(GetPathClause(SESSION_ID_DEPTH, nodePath.Cstr()));
		sid = sid.mid(0, sid.find('/') );
		
		switch (pd)
		{
		case CHANNEL_DEPTH:
			{
				QString cdata = QString::fromUtf8(GetPathClause(CHANNELDATA_DEPTH, nodePath.Cstr()));
				cdata = cdata.mid(0, cdata.find('/') );
				if (cdata == "channeldata")
				{
					QString channel = QString::fromUtf8(GetPathClause(CHANNEL_DEPTH, nodePath.Cstr()));
					if (channel.find('/') >= 0)
					{
						channel = channel.mid(0, channel.find('/') );
					}
					if (channel != "")
					{
						// user parted channel
						RemoveChannel(sid, channel);
					}
				}
				break;
			}
			
		}
	}
}

void
NetClient::HandleUniAddMessage(String nodePath, MessageRef ref)
{
	QString cdata;
	PRINT("UniShare: AddMessage - node = %s\n", nodePath.Cstr());
	int pd = GetPathDepth(nodePath.Cstr());
	if (pd >= USER_NAME_DEPTH)
	{
		MessageRef tmpRef;
		if (ref()->FindMessage(nodePath.Cstr(), tmpRef) == B_OK)
		{
			const Message * pmsg = tmpRef.GetItemPointer();
			
			QString sid = QString::fromUtf8(GetPathClause(SESSION_ID_DEPTH, nodePath.Cstr()));
			sid = sid.mid(0, sid.find('/') );
			
			switch (pd)
			{
			case USER_NAME_DEPTH:
				{
					PRINT("UniShare: PathDepth == USER_NAME_DEPTH\n");
					QString nodeName = QString::fromUtf8(GetPathClause(USER_NAME_DEPTH, nodePath.Cstr()));
					if (nodeName.lower().left(10) == "serverinfo")
					{
						int64 rtime;
						String user, newid, oldid;
						pmsg->FindInt64("registertime", &rtime);
						pmsg->FindString("user", user);
						pmsg->FindString("session", newid);
						QString qUser = QString::fromUtf8(user.Cstr());
						if (pmsg->FindString("oldid", oldid) == B_OK)
						{
							QString nid = QString::fromUtf8(newid.Cstr());
							QString oid = QString::fromUtf8(oldid.Cstr());
							emit UserIDChanged(oid, nid);
						}
						if (
							( gWin->GetUserName() == qUser ) &&
							( gWin->GetRegisterTime(qUser) <= rtime )
						)
						{
							// Collide nick
							MessageRef col = GetMessageFromPool(RegisterFail);
							if (col())
							{
								QString to("/*/");
								to += sid;
								to += "/unishare";
								col()->AddString(PR_NAME_KEYS, (const char *) to.utf8());
								col()->AddString("name", (const char *) gWin->GetUserName().utf8() );
								col()->AddInt64("registertime", gWin->GetRegisterTime( gWin->GetUserName() ) );
								SendMessageToSessions(col);
							}
						}
					}
					break;
				}
			case CHANNEL_DEPTH:
				{
					PRINT("UniShare: PathDepth == CHANNEL_DEPTH\n");
					cdata = QString::fromUtf8(GetPathClause(CHANNELDATA_DEPTH, nodePath.Cstr()));
					cdata = cdata.mid(0, cdata.find('/') );
					if (cdata == "channeldata")
					{
						QString channel = QString::fromUtf8(GetPathClause(CHANNEL_DEPTH, nodePath.Cstr()));
						if (channel.find('/') >= 0)
						{
							channel = channel.mid(0, channel.find('/') );
						}
						if (channel != "")
						{
							// user joined channel
							AddChannel(sid, channel);
							String topic, owner, admins;
							bool pub;
							if (pmsg->FindString("owner", owner) == B_OK)
								emit ChannelOwner(channel, sid, QString::fromUtf8(owner.Cstr()));
							if (pmsg->FindString("admins", admins) == B_OK)
								emit ChannelAdmins(channel, sid, QString::fromUtf8(admins.Cstr()));
							if (pmsg->FindString("topic", topic) == B_OK)
								emit ChannelTopic(channel, sid, QString::fromUtf8(topic.Cstr()));
							if (pmsg->FindBool("public", &pub) == B_OK)
								emit ChannelPublic(channel, sid, pub);
						}
					}

					break;
				}
			case CHANNELINFO_DEPTH:
				{
					PRINT("UniShare: PathDepth == CHANNELINFO_DEPTH\n");
					break;
				}
			}
		}
	}
}

void
NetClient::AddChannel(QString sid, QString channel)
{
	MessageRef mChannel;
	fChannelLock.lock();
	if ( fChannels->FindMessage((const char *) channel.utf8(), mChannel) == B_OK)
	{
		mChannel()->AddBool(sid.latin1(), true);
	}
	else
	{
		emit ChannelAdded(channel, sid, GetCurrentTime64());
		MessageRef mChannel = GetMessageFromPool();
		mChannel()->AddBool(sid.latin1(), true);
		fChannels->AddMessage((const char *) channel.utf8(), mChannel);
	}
	fChannelLock.unlock();
}

void
NetClient::RemoveChannel(QString sid, QString channel)
{
	MessageRef mChannel;
	fChannelLock.lock();
	if ( fChannels->FindMessage((const char *) channel.utf8(), mChannel) == B_OK)
	{
		mChannel()->RemoveName(sid.latin1());
		if (mChannel()->CountNames(B_MESSAGE_TYPE) == 0)
		{
			// Last user parted, remove channel entry
			fChannels->RemoveName((const char *) channel.utf8());
		}
	}
	fChannelLock.unlock();
}

QString *
NetClient::GetChannelList()
{
	fChannelLock.lock();
	int n = fChannels->CountNames(B_MESSAGE_TYPE);
	QString * qChannels = new QString[n];
	int i = 0;
	String channel;
	MessageFieldNameIterator iter = fChannels->GetFieldNameIterator(B_MESSAGE_TYPE);
	while (iter.GetNextFieldName(channel) == B_OK)
	{
		qChannels[i++] = QString::fromUtf8(channel.Cstr());
	}
	fChannelLock.unlock();
	return qChannels;
}

QString *
NetClient::GetChannelUsers(QString channel)
{
	MessageRef mChannel;
	fChannelLock.lock();
	QString * users = NULL;
	if (fChannels->FindMessage((const char *) channel.utf8(), mChannel) == B_OK)
	{
		int n = mChannel()->CountNames(B_BOOL_TYPE);
		users = new QString[n];
		int i = 0;
		String user;
		MessageFieldNameIterator iter = mChannel()->GetFieldNameIterator(B_BOOL_TYPE);
		while (iter.GetNextFieldName(user) == B_OK)
		{
			users[i++] = QString::fromUtf8(user.Cstr());
		}
	}
	fChannelLock.unlock();
	return users;
}

int
NetClient::GetChannelCount()
{
	fChannelLock.lock();
	int n = fChannels->CountNames(B_MESSAGE_TYPE);
	fChannelLock.unlock();
	return n;
}

int
NetClient::GetUserCount(QString channel)
{
	fChannelLock.lock();
	int n = 0;
	MessageRef mChannel;
	if (fChannels->FindMessage((const char *) channel.utf8(), mChannel) == B_OK)
	{
		n = mChannel()->CountNames(B_BOOL_TYPE);
	}
	fChannelLock.unlock();
	return n;
}

void
NetClient::HandleBeAddMessage(String nodePath, MessageRef ref)
{
	int pd = GetPathDepth(nodePath.Cstr());
	if (pd >= USER_NAME_DEPTH)
	{
		MessageRef tmpRef;
		if (ref()->FindMessage(nodePath.Cstr(), tmpRef) == B_OK)
		{
			const Message * pmsg = tmpRef.GetItemPointer();
			QString sid = GetPathClause(SESSION_ID_DEPTH, nodePath.Cstr());
			sid = sid.left(sid.find('/'));
			switch (pd)
			{
			case USER_NAME_DEPTH:
				{
					QString hostName = QString::fromUtf8(GetPathClause(NetClient::HOST_NAME_DEPTH, nodePath.Cstr()));
					hostName = hostName.left(hostName.find('/'));
					
					WUserRef user = FindUser(sid);
					if (!user())	// doesn't exist
					{
						user = CreateUser(sid);
						if (!user())	// couldn't create?
							break;	// oh well
						user()->SetUserHostName(hostName);
					}
					
					QString nodeName = QString::fromUtf8(GetPathClause(USER_NAME_DEPTH, nodePath.Cstr()));
					if (nodeName.lower().left(4) == "name")
					{
						QString oldname = user()->GetUserName();
						user()->InitName(pmsg); 
						if (oldname != user()->GetUserName())
							emit UserNameChanged(sid, oldname, user()->GetUserName());
					}
					else if (nodeName.lower().left(10) == "userstatus")
					{
						QString oldstatus = user()->GetStatus();
						user()->InitStatus(pmsg);
						if (oldstatus != user()->GetStatus())
							emit UserStatusChanged(sid, user()->GetUserName(), user()->GetStatus());
					}
					else if (nodeName.lower().left(11) == "uploadstats")
					{
						user()->InitUploadStats(pmsg);
					}
					else if (nodeName.lower().left(9) == "bandwidth")
					{
						user()->InitBandwidth(pmsg);
					}
					else if (nodeName.lower().left(9) == "filecount")
					{
						user()->InitFileCount(pmsg);
					}
					else if (nodeName.lower().left(5) == "fires")
					{
						user()->SetFirewalled(true);
					}
					else if (nodeName.lower().left(5) == "files")
					{
						user()->SetFirewalled(false);
					}
				}
				break;
				
			case FILE_INFO_DEPTH:
				{
					QString fileName = QString::fromUtf8(GetPathClause(FILE_INFO_DEPTH, nodePath.Cstr()));
					
					MessageRef unpacked = InflateMessage(tmpRef);
					if (unpacked())
						emit AddFile(sid, fileName, (GetPathClause(USER_NAME_DEPTH, nodePath.Cstr())[2] == 'r')? true : false, unpacked);
					break;
				}
			}
		}
	}
}

void
NetClient::HandleResultMessage(MessageRef & ref)
{
	String nodePath;
	// remove all the items that need to be removed
	for (int i = 0; (ref()->FindString(PR_NAME_REMOVED_DATAITEMS, i, nodePath) == B_OK); i++)
	{
		QString prot = GetPathClause(BESHARE_HOME_DEPTH, nodePath.Cstr());
		prot = prot.mid(0, prot.find('/') );
		if (prot == "beshare")
		{
			HandleBeRemoveMessage(nodePath);
		}
		else if (prot == "unishare")
		{
			HandleUniRemoveMessage(nodePath);
		}
	}

	// look for addition messages
	MessageFieldNameIterator iter = ref()->GetFieldNameIterator(B_MESSAGE_TYPE);
	while (iter.GetNextFieldName(nodePath) == B_OK)
	{
		QString prot = GetPathClause(BESHARE_HOME_DEPTH, nodePath.Cstr());
		prot = prot.mid(0, prot.find('/') );
		if (prot == "beshare")
		{
			HandleBeAddMessage(nodePath, ref);
		}
		else if (prot == "unishare")
		{
			HandleUniAddMessage(nodePath, ref);
		}
	}
}

void
NetClient::HandleParameters(MessageRef & next)
{
	PRINT("PR_RESULT_PARAMETERS received\n");
	PRINT("Extracting session id\n");
	const char * sessionRoot;

	if (next()->FindString(PR_NAME_SESSION_ROOT, &sessionRoot) == B_OK)
	{
		// returns something like
		// "ip.ip.ip.ip/number" (eg: "/127.172.172.172/1234")
		const char * id = strrchr(sessionRoot, '/');	// get last slash
		if (id)
		{
			id++; // Skip '/'
			fOldID = fSessionID;
			fSessionID = id;

			if (gWin->fDLWindow)
			{
				// Update Local Session ID in Download Window
				gWin->fDLWindow->SetLocalID(fSessionID);
			}

			MessageRef uc(GetMessageFromPool());
			if (uc())
			{
				uc()->AddInt64("registertime", gWin->GetRegisterTime(fUserName));
				uc()->AddString("session", (const char *) fSessionID.utf8());
				if ((fOldID != QString::null) && (fOldID != fSessionID))
				{
					uc()->AddString("oldid", (const char *) fOldID.utf8());
				}
				uc()->AddString("name", (const char *) fUserName.utf8());
		
				SetNodeValue("unishare/serverinfo", uc);
			}

			if ((fOldID != QString::null) && (fOldID != fSessionID))
			{
				emit UserIDChanged(fOldID, fSessionID);
				fOldID = fSessionID;
			}

			WString wSessionID = fSessionID;
			PRINT("My ID is: %S\n", wSessionID.getBuffer());

			gWin->setCaption( tr("Unizone - User #%1 on %2").arg(fSessionID).arg(GetServer()) );
		}
	}
}

void
NetClient::SendChatText(QString target, QString text)
{
	if (IsInternalThreadRunning())
	{
		MessageRef chat(GetMessageFromPool(NEW_CHAT_TEXT));
		if (chat())
		{
			QString tostr = "/*/";
			tostr += target;
			tostr += "/beshare";
			chat()->AddString(PR_NAME_KEYS, (const char *) tostr.utf8());
			chat()->AddString("session", (const char *) fSessionID.utf8());
			chat()->AddString("text", (const char *) text.utf8());
			if (target != "*")
				chat()->AddBool("private", true);
			SendMessageToSessions(chat);
		}
	}
}

void
NetClient::SendPing(QString target)
{
	if (IsInternalThreadRunning())
	{
		MessageRef ping(GetMessageFromPool(PING));
		if (ping())
		{
			QString to("/*/");
			to += target;
			to += "/beshare";
			ping()->AddString(PR_NAME_KEYS, (const char *) to.utf8());
			ping()->AddString("session", (const char *) LocalSessionID().utf8());
			ping()->AddInt64("when", GetCurrentTime64());
			SendMessageToSessions(ping);
		}
	}
}

void
NetClient::SetUserName(QString user)
{
	fUserName = user;
	// change the user name
	MessageRef ref(GetMessageFromPool());
	if (ref())
	{
		QString version = tr("Unizone (English)");
		QCString vstring = WinShareVersionString().utf8();
		ref()->AddString("name", (const char *) user.utf8()); // <postmaster@raasu.org> 20021001
		ref()->AddInt32("port", fPort);
		ref()->AddInt64("installid", 0);
		ref()->AddString("version_name", (const char *) version.utf8());	// "secret" WinShare version data (so I don't have to ping Win/LinShare users
		ref()->AddString("version_num", (const char *) vstring);
		ref()->AddBool("supports_partial_hashing", true);		// 64kB hash sizes
		ref()->AddBool("firewalled", gWin->fSettings->GetFirewalled()); // is firewalled user, needed if no files shared

		SetNodeValue("beshare/name", ref);
	}
}

void
NetClient::SetUserStatus(QString status)
{
	MessageRef ref(GetMessageFromPool());
	if (ref())
	{
		ref()->AddString("userstatus", (const char *) status.utf8()); // <postmaster@raasu.org> 20021001
		SetNodeValue("beshare/userstatus", ref);
	}
}

void
NetClient::SetConnection(QString connection)
{
	int32 bps;
	MessageRef ref(GetMessageFromPool());
	if (ref())
	{
		bps = BandwidthToBytes(connection);

		ref()->AddString("label", (const char *) connection.utf8());
		ref()->AddInt32("bps", bps);

		SetNodeValue("beshare/bandwidth", ref);
	}
}

void
NetClient::SetNodeValue(const char * node, MessageRef & val)
{
	MessageRef ref(GetMessageFromPool(PR_COMMAND_SETDATA));
	if (ref())
	{
		ref()->AddMessage(node, val);
		SendMessageToSessions(ref);
	}
}

void
NetClient::SetFileCount(int32 count)
{
	MessageRef fc(GetMessageFromPool());
	fc()->AddInt32("filecount", count);
	SetNodeValue("beshare/filecount", fc);
}

void
NetClient::SetLoad(int32 num, int32 max)
{
	MessageRef load(GetMessageFromPool());
	if (load())
	{
		load()->AddInt32("cur", num);
		load()->AddInt32("max", max);
		SetNodeValue("beshare/uploadstats", load);
	}
}

QString
NetClient::GetServerIP()
{
	QString ip = "0.0.0.0";
	uint32 address;
	char host[16];
	address = GetHostByName(fServer.latin1());
	if (address > 0)
	{
		Inet_NtoA(address, host);
		ip = host;
	}
	return ip;
}

