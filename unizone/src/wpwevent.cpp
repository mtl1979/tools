#include "wpwevent.h"
#include "user.h"
#include "privatewindowimpl.h"
#include "debugimpl.h"
#include "platform.h"

WPWEvent::WPWEvent(int type, WUserMap & users, const QString & msg)
		: QCustomEvent(type)
{
	fWant = false;
	if (type == WPWEvent::TextEvent)
	{
		fWant = true;	// this way we'll get an error string in the private window if
		// an invalid command is used
		// everything is stuck into one string
		if (msg[0] == '/' && !msg.startsWith("//"))
		{
			// just forward the message
			fMsg = msg;
		}
		else
		{
			QString smsg = msg;
			if (smsg.startsWith("//"))
				smsg.replace(0, 2, "/");
			fMsg = "/msg ";
			for (WUserIter it = users.begin(); it != users.end(); it++)
			{
				fMsg += (*it).second()->GetUserID();
				fMsg += ",";
			}
			fMsg.truncate(fMsg.length() - 1);
			fMsg += " ";
			fMsg += smsg;
			
			wchar_t * wText = qStringToWideChar(fMsg);
			PRINT("Sending text: %S\n", wText);
			delete [] wText;
		}
	}
	else
	{
		fMsg = msg;
	}
}

WPWEvent::WPWEvent(int type, const QString & msg)
		: QCustomEvent(type)
{
	fMsg = msg;
}