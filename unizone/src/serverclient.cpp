#include "serverclient.h"
#include "debugimpl.h"
#include "global.h"
#include "winsharewindow.h"
#include "iogateway/PlainTextMessageIOGateway.h"
#include "util/StringTokenizer.h"

ServerClient::ServerClient(QObject *owner)
{
	setName( "ServerClient" );
	qmtt = new QMessageTransceiverThread();
	CHECK_PTR(qmtt);

	connect(qmtt, SIGNAL(MessageReceived(MessageRef, const String &)),
			this, SLOT(MessageReceived(MessageRef, const String &)));
	connect(qmtt, SIGNAL(SessionConnected(const String &)),
			this, SLOT(SessionConnected(const String &)));
	connect(qmtt, SIGNAL(SessionDetached(const String &)),
			this, SLOT(SessionDetached(const String &)));
}

ServerClient::~ServerClient()
{
}

void
ServerClient::MessageReceived(MessageRef msg, const String &sessionID)
{
	if (msg())
	{
		String nstr;
		for (int i = 0; msg()->FindString(PR_NAME_TEXT_LINE, i, nstr) == B_OK; i++)
		{
			PRINT("UPDATESERVER: %s\n", nstr.Cstr());
			int hind = nstr.IndexOf("#");
			if (hind >= 0)
				nstr = nstr.Substring(0, hind);
			if (nstr.StartsWith("beshare_"))
			{
				StringTokenizer tok(nstr() + 8, "=");
				const char * param = tok.GetNextToken();
				if (param)
				{
					const char * val = tok.GetRemainderOfString();
					String valstr(val ? val : "");
					gWin->GotUpdateCmd(String(param).Trim().Cstr(), valstr.Trim().Cstr());
				}
			}
		}
	}
}

void
ServerClient::SessionConnected(const String &sessionID)
{
	MessageRef msgref(new Message, NULL);
	if (msgref())
	{
		msgref()->AddString(PR_NAME_TEXT_LINE, "GET /servers.txt HTTP/1.1\nUser-Agent: Unizone/1.2\nHost: beshare.tycomsystems.com\n\n");
		qmtt->SendMessageToSessions(msgref);
	}
}

void
ServerClient::SessionDetached(const String &sessionID)
{
	qmtt->Reset();
}

void
ServerClient::Disconnect()
{
	PRINT("DISCONNECT\n");
	if (qmtt->IsInternalThreadRunning()) 
	{
		qmtt->ShutdownInternalThread();
		qmtt->Reset(); 
	}
}
