/* This file is Copyright 2003 Level Control Systems.  See the included LICENSE.txt file for details. */

#include "qtsupport/QAcceptSocketsThread.h"

namespace muscle {

static const uint32 QMTT_SIGNAL_EVENT = 8360446;  // why yes, this is a completely arbitrary number

QAcceptSocketsThread :: QAcceptSocketsThread()
{
   setName("QAcceptSocketsThread");
   // empty
}

QAcceptSocketsThread :: ~QAcceptSocketsThread()
{
   // empty
}

void QAcceptSocketsThread :: SignalOwner()
{
   QCustomEvent * evt = newnothrow QCustomEvent(QMTT_SIGNAL_EVENT);
   if (evt) QThread::postEvent(this, evt);
       else WARN_OUT_OF_MEMORY;
}

bool QAcceptSocketsThread :: event(QEvent * event)
{
   if (event->type() == QMTT_SIGNAL_EVENT)
   {
      MessageRef next;

      // Check for any new messages from our HTTP thread
      while(GetNextReplyFromInternalThread(next) >= 0)
      {
         switch(next()->what)
         {
            case AST_EVENT_NEW_SOCKET_ACCEPTED:      
            {
               GenericRef tag;
               if (next()->FindTag(AST_NAME_SOCKET, tag) == B_NO_ERROR)
               {
                  SocketHolderRef sref(tag, NULL);
                  if (sref()) emit ConnectionAccepted(sref);
               }
            }
            break;
         }
      }
      return true;
   }
   else return QObject::event(event);
}

};  // end namespace muscle