#ifndef NETCLIENT_H
#define NETCLIENT_H


// #include "system/MessageTransceiverThread.h"
#include "qtsupport/QMessageTransceiverThread.h"

#include "support/MuscleSupport.h"

using namespace muscle;

#include <qobject.h>

#include "user.h"
class NetClient : public QObject 
{
	Q_OBJECT
public:
	NetClient(QObject * owner);
	virtual ~NetClient();
	
	// <postmaster@raasu.org> -- Add support for port numbers
	status_t Connect(const QString &server, uint16 port);
	status_t Connect(const QString &server);
	void Disconnect();
	QString GetServer() { return fServer; } // Get current server hostname or ip address if hostname isn't available
	QString GetServerIP();					// Get current server IP address
	uint32 GetServerPort() { return fServerPort; } // Get current server port number
	
	bool IsConnected() const;
	
	void AddSubscription(const QString & str, bool q = false);	// if "q" is true, u won't get an initial response
	void RemoveSubscription(const QString & str);
	
	void SendChatText(const QString &target, const QString &text);
	void SendPing(const QString &target);
	
	void SetUserName(const QString &user); 	// <postmaster@raasu.org> 20021001
	void SetUserStatus(const QString &status);		//
	void SetConnection(const QString &connection);
	void SetPort(uint32 port) { fPort = port; }
	void SetFileCount(int32 count);
	void SetLoad(int32 num, int32 max);
	
	status_t SendMessageToSessions(MessageRef msgRef, const char * optDistPath = NULL);
	void SetNodeValue(const char * node, MessageRef & val); // set the Message of a node
	
	QString * GetChannelList();
	int GetChannelCount();
	
	QString * GetChannelUsers(const QString &channel);
	int GetUserCount(const QString &channel);
	
	// events
	enum
	{
		/* SignalEvent = QEvent::User + 4000, */
			SESSION_ATTACHED = QEvent::User + 4000,	// just some constant
			SESSION_CONNECTED,
			DISCONNECTED,
			MESSAGE_RECEIVED
	};
	// client messages
	enum 
	{
		CONNECTED_TO_SERVER = 0,
			DISCONNECTED_FROM_SERVER,
			NEW_CHAT_TEXT,
			CONNECT_BACK_REQUEST,
			CHECK_FILE_COUNT,
			PING,
			PONG,
			SCAN_THREAD_REPORT
	}; 
	
	// path matching -- BeShare
	enum 
	{
		ROOT_DEPTH = 0, 		// root node
			HOST_NAME_DEPTH,		 
			SESSION_ID_DEPTH,
			BESHARE_HOME_DEPTH, 	// used to separate our stuff from other (non-BeShare) data on the same server
			USER_NAME_DEPTH,		// user's handle node would be found here
			FILE_INFO_DEPTH 		// user's shared file list is here
	};
	
	// path matching -- UniShare
	enum
	{
		UNISHARE_HOME_DEPTH = BESHARE_HOME_DEPTH,
			CHANNELDATA_DEPTH,
			CHANNEL_DEPTH,
			CHANNELINFO_DEPTH
	};
	
	// UniShare event codes
	enum
	{
		ClientConnected = 'UsCo',
			RegisterFail,
			ClientDisconnected,
			ChannelCreated,
			ChannelText,
			ChannelData,
			ChannelInvite,
			ChannelKick,
			ChannelMaster,
			ChannelJoin,
			ChannelPart,
			ChannelSetTopic,
			ChannelSetPublic
	};
	
	QString LocalSessionID() const;
	
	
	void HandleParameters(MessageRef & next);
	void HandleResultMessage(MessageRef & ref);
	
	bool ExistUser(const QString &sid);
	// this is idential to ExistUser() in that it will return
	// NULL (ExistUser() returns false) if the user is not found, but unlike
	// ExistUser(), it will return a pointer to the found user.
	WUserRef FindUser(const QString &sid);
	// Find users by IP address
	void FindUsersByIP(WUserMap & umap, const QString &ip);
	WUserRef FindUserByIPandPort(const QString &ip, uint32 port);
	// create a new user
	WUserRef CreateUser(const QString &sessionID);
	// deletes a user, including removing from the list view
	void RemoveUser(const QString &sessionID);
	
	WUserMap & Users() { return fUsers; }
	
	// forwarders

	void Reset();
	status_t SetOutgoingMessageEncoding(int32 encoding, const char * optDistPath = NULL);
	status_t WaitForInternalThreadToExit();

signals:
	void UserDisconnected(const QString &, const QString &);
	void UserConnected(const QString &);
	void UserNameChanged(const QString &, const QString &, const QString &);
	void DisconnectedFromServer();
	void UserStatusChanged(const QString &, const QString &, const QString &);
	void UserIDChanged(const QString &, const QString &);
	void UserHostName(const QString &, const QString &);
	
	void RemoveFile(const QString &, const QString &);
	void AddFile(const QString &, const QString &, bool, MessageRef);
	
	void ChannelTopic(const QString &, const QString &, const QString &);
	void ChannelAdmins(const QString &, const QString &, const QString &);
	void ChannelAdded(const QString &, const QString &, int64);
	void ChannelPublic(const QString &, const QString &, bool);
	void ChannelOwner(const QString &, const QString &, const QString &);
	
private:
	uint32 fPort, fServerPort;
	QString fSessionID, fServer;
	QString fOldID; 		// Old id for persistent channel admin/owner state
	QString fUserName;
	QObject * fOwner;
	WUserMap fUsers;		// a list of users
	MessageRef fChannels;	// channel database
	
	void HandleBeRemoveMessage(const String &nodePath);
	void HandleBeAddMessage(const String &nodePath, MessageRef ref);
	
	void HandleUniRemoveMessage(const String &nodePath);
	void HandleUniAddMessage(const String &nodePath, MessageRef ref);
	
	void AddChannel(const QString &sid, const QString &channel);
	void RemoveChannel(const QString &sid, const QString &channel);

	void SendSignal(int signal);
	
	mutable QMutex fChannelLock;

	int timerID;

	QMessageTransceiverThread *qmtt;

private slots:

	void MessageReceived(MessageRef msg, const String & sessionID);

	void SessionAttached(const String & sessionID);
	void SessionDetached(const String & sessionID);
   
	void SessionAccepted(const String & sessionID, uint16 port);

	void SessionConnected(const String & sessionID);
	void SessionDisconnected(const String & sessionID);

	void FactoryAttached(uint16 port);
	void FactoryDetached(uint16 port);

	void ServerExited();

	void OutputQueuesDrained(MessageRef ref);

protected:

	virtual void timerEvent(QTimerEvent *);
};


#endif	// NETCLIENT_H

