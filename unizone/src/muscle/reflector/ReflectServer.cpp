/* This file is Copyright 2002 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include "reflector/ReflectServer.h"
#include "reflector/StorageReflectConstants.h"
#include "util/NetworkUtilityFunctions.h"
#include "util/MemoryAllocator.h"

#ifdef MUSCLE_ENABLE_MEMORY_TRACKING
# include "system/GlobalMemoryAllocator.h"
#endif

namespace muscle {

status_t
ReflectServer ::
AddNewSession(AbstractReflectSessionRef ref, int s)
{
   AbstractReflectSession * newSession = ref();
   if (newSession == NULL) return B_ERROR;

   newSession->_owner = this;  // in case CreateGateway() needs to use the owner

   // Create the gateway object for this session, if it isn't already set up
   if (s >= 0)
   {
      AbstractMessageIOGatewayRef gatewayRef = newSession->GetGatewayRef();
      if (gatewayRef() == NULL) gatewayRef.SetRef(newSession->CreateGateway(), NULL);
      if (gatewayRef())
      {
         // create the new DataIO for the gateway; this must always be done on the fly
         // since it depends on the socket being used.
         DataIO * io = newSession->CreateDataIO(s);
         if (io) 
         {
            // success!
            gatewayRef()->SetDataIO(DataIORef(io, NULL));
            newSession->SetGateway(gatewayRef);
         }
         else return B_ERROR;
      }
      else return B_ERROR;
   }

   // Set our hostname (IP address) string if it isn't already set
   if (newSession->_hostName.Length() == 0)
   {
      int socket = GetSocketFor(newSession);
      if (socket >= 0)
      {
         uint32 ip = GetPeerIPAddress(socket);
         const String * remapString = _remapIPs.Get(ip);
         newSession->_hostName = remapString ? *remapString : ((ip > 0) ? Inet_NtoA(ip) : String("<unknown>"));
      }
      else newSession->_hostName = "<local>";
   }

   return AddNewSession(ref);
}

status_t
ReflectServer ::
AddNewConnectSession(AbstractReflectSessionRef ref, uint32 destIP, uint16 port)
{
   AbstractReflectSession * session = ref();
   if (session)
   {
      bool isReady;
      int socket = ConnectAsync(destIP, port, isReady);
      if (socket >= 0)
      {
         session->_hostName = (destIP > 0) ? Inet_NtoA(destIP) : String("<unknown>");
         session->_connectingAsync = (isReady == false);
         status_t ret = AddNewSession(ref, socket);
         if (ret == B_NO_ERROR)
         {
            if (isReady) session->AsyncConnectCompleted();
            return B_NO_ERROR;
         }
      }
   }
   return B_ERROR;
}

status_t
ReflectServer ::
AddNewSession(AbstractReflectSessionRef ref)
{
   AbstractReflectSession * newSession = ref.GetItemPointer();
   if ((newSession)&&(_sessions.Put(newSession->GetSessionIDString(), ref) == B_NO_ERROR))
   {
      newSession->_owner = this;
      if (newSession->AttachedToServer() == B_NO_ERROR)
      {
         if (_doLogging) LogTime(MUSCLE_LOG_INFO, "New %s [%s] from [%s]@%i (%lu total)\n", newSession->GetTypeName(), newSession->GetSessionIDString(), newSession->GetHostName(), newSession->GetPort(), _sessions.GetNumItems());
         return B_NO_ERROR;
      }
      else 
      {
         newSession->AboutToDetachFromServer();  // well, it *was* attached, if only for a moment
         if (_doLogging) LogTime(MUSCLE_LOG_INFO, "%s [%s] from [%s]@%i aborted startup (%lu left)\n", newSession->GetTypeName(), newSession->GetSessionIDString(), newSession->GetHostName(), newSession->GetPort(), _sessions.GetNumItems()-1);
      }
      newSession->_owner = NULL;
      (void) _sessions.Remove(newSession->GetSessionIDString());
   }
   return B_ERROR;
}


ReflectServer :: ReflectServer(UsageLimitProxyMemoryAllocator * optMemoryUsageTracker) : _keepServerGoing(true), _startedAt(0), _doLogging(true), _watchMemUsage(optMemoryUsageTracker)
{
   // make sure _lameDuckSessions has plenty of memory available in advance (we need might need it in a tight spot later!)
   _lameDuckSessions.EnsureSize(256);
   _sessions.SetKeyCompareFunction(CStringCompareFunc);
}

ReflectServer :: ~ReflectServer()
{
   // empty
}

void
ReflectServer :: Cleanup()
{
   // Detach all sessions
   {
      HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
      const char * nextKey;
      AbstractReflectSessionRef nextValue;
      while((iter.GetNextKey(nextKey) == B_NO_ERROR)&&(iter.GetNextValue(nextValue) == B_NO_ERROR)) 
      {
         if (nextValue()) 
         {
            nextValue()->AboutToDetachFromServer();
            nextValue()->_owner = NULL;
            _lameDuckSessions.AddTail(nextValue);  // we'll delete it below
            _sessions.Remove(nextKey);  // but prevent other sessions from accessing it now that it's detached
         }
      }
   }
  
   // Detach all factories
   RemoveAcceptFactory(0);

   // This will dereference everything so they can be safely deleted here
   _lameDuckSessions.Clear();
   _lameDuckFactories.Clear();
}

status_t
ReflectServer ::
ReadyToRun()
{
   return B_NO_ERROR;
}

const char *
ReflectServer ::
GetServerName() const
{
   return "MUSCLE";
}

/** Makes sure the given policy has its BeginIO() called, if necessary, and returns it */
uint32
ReflectServer :: 
CheckPolicy(Hashtable<PolicyRef, bool> & policies, PolicyRef policyRef, const PolicyHolder & ph, uint64 now) const
{
   AbstractSessionIOPolicy * p = policyRef();
   if (p)
   {
      // Any policy that is found attached to a session goes into our temporary policy set
      (void) policies.Put(policyRef, true);

      // If the session is ready, and BeginIO() hasn't been called on this policy already, do so now
      if ((ph.GetSession())&&(p->_hasBegun == false))
      {
         p->BeginIO(now); 
         p->_hasBegun = true;
      }
   }
   return ((ph.GetSession())&&((p == NULL)||(p->OkayToTransfer(ph)))) ? MUSCLE_NO_LIMIT : 0;
}

status_t 
ReflectServer ::
ServerProcessLoop()
{
   _startedAt = GetCurrentTime64();

   if (_doLogging)
   {
      LogTime(MUSCLE_LOG_DEBUG, "This %s server was compiled on " __DATE__ " " __TIME__ "\n", GetServerName());
      LogTime(MUSCLE_LOG_DEBUG, "The server was compiled with MUSCLE version %s\n", MUSCLE_VERSION_STRING);
   }

   if (ReadyToRun() != B_NO_ERROR) 
   {
      if (_doLogging) LogTime(MUSCLE_LOG_CRITICALERROR, "Server:  ReadyToRun() failed, aborting.\n");
      return B_ERROR;
   }

   // Print an informative startup message
   if (_doLogging)
   {
      int numFuncs = _factories.GetNumItems();
      LogTime(MUSCLE_LOG_INFO, "%s Server is active", GetServerName()); 
      if (numFuncs > 0)
      {
         Log(MUSCLE_LOG_INFO, " and listening on port%s ", (numFuncs > 1) ? "s" : "");
         HashtableIterator<uint16, ReflectSessionFactoryRef> iter = _factories.GetIterator();
         uint16 port;
         int which=0;
         while(iter.GetNextKey(port) == B_NO_ERROR) Log(MUSCLE_LOG_INFO, "%u%s", port, (++which<numFuncs)?",":"");
      }
      else Log(MUSCLE_LOG_INFO, " but not listening to any ports");
      Log(MUSCLE_LOG_INFO, ".\n");
   }

   // The primary event loop for any MUSCLE-based server!
   // These variables are used as scratch space, but are declared outside the loop to avoid having to reinitialize them all the time.
   Hashtable<PolicyRef, bool> policies;
   fd_set readSet;
   fd_set writeSet;
   while(ClearLameDucks() == B_NO_ERROR)
   {
      // Initialize our fd sets of events-to-watch-for
      FD_ZERO(&readSet);
      FD_ZERO(&writeSet);
      int maxSocket = -1;
      uint64 nextPulseAt = MUSCLE_TIME_NEVER; // running minimum of everything that wants to be Pulse()'d

      // Set up fd set entries and Pulse() timing info for all our different components
      {
         const uint64 now = GetCurrentTime64();

         // Set up the session-acceptors and their associated session-factories
         if (_factories.GetNumItems() > 0)
         {
            HashtableIterator<uint16, ReflectSessionFactoryRef> iter = _factories.GetIterator();
            ReflectSessionFactoryRef * next; 
            while((next = iter.GetNextValue()) != NULL)
            {
               ReflectSessionFactory * factory = (*next)();
               int nextAcceptSocket = factory->_socket;
               FD_SET(nextAcceptSocket, &readSet);
               if (nextAcceptSocket > maxSocket) maxSocket = nextAcceptSocket;
               if (factory) CallGetPulseTimeAux(*factory, now, nextPulseAt);
            }
         }

         // Set up the sessions and their associated IO-gateways
         if (_sessions.GetNumItems() > 0)
         {
            AbstractReflectSessionRef * nextRef;
            HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
            while((nextRef = iter.GetNextValue()) != NULL)
            {
               AbstractReflectSession * session = nextRef->GetItemPointer();
               if (session)
               {
                  session->_maxInputChunk = session->_maxOutputChunk = 0;
                  AbstractMessageIOGateway * g = session->GetGateway();
                  if (g)
                  {
                     int sessionSocket = GetSocketFor(session);
                     if (sessionSocket >= 0)
                     {
                        bool in, out;
                        if (session->_connectingAsync) 
                        {
                           in  = false;
                           out = true;  // so we can watch for the async-connect event
                        }
                        else
                        {
                           session->_maxInputChunk  = CheckPolicy(policies, session->GetInputPolicy(),  PolicyHolder(g->IsReadyForInput()  ? session : NULL, true),  now);
                           session->_maxOutputChunk = CheckPolicy(policies, session->GetOutputPolicy(), PolicyHolder(g->HasBytesToOutput() ? session : NULL, false), now);
                           in  = (session->_maxInputChunk  > 0);
                           out = (session->_maxOutputChunk > 0);
                        }

                        if (in) FD_SET(sessionSocket, &readSet);
                        if (out) 
                        {
                           FD_SET(sessionSocket, &writeSet);
                           if (session->_lastByteOutputAt == 0) session->_lastByteOutputAt = now;  // start timing....
                        }
                        else session->_lastByteOutputAt = 0;  // stop timing

                        if (((in)||(out))&&(sessionSocket > maxSocket)) maxSocket = sessionSocket;
                     }
                     CallGetPulseTimeAux(*g, now, nextPulseAt);
                  }
                  CallGetPulseTimeAux(*session, now, nextPulseAt);
               }
            }
            CallGetPulseTimeAux(*this, now, nextPulseAt);
         }

         // Set up the Session IO Policies
         if (policies.GetNumItems() > 0)
         {
            // Now that the policies know *who* amongst their policyholders will be reading/writing,
            // let's ask each activated policy *how much* each policyholder should be allowed to read/write.
            {
               AbstractReflectSessionRef * nextRef;
               HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
               while((nextRef = iter.GetNextValue()) != NULL)
               {
                  AbstractReflectSession * session = nextRef->GetItemPointer();
                  if (session)
                  {
                     AbstractSessionIOPolicy * inPolicy  = session->GetInputPolicy()();
                     AbstractSessionIOPolicy * outPolicy = session->GetOutputPolicy()();
                     if ((inPolicy)&&( session->_maxInputChunk  > 0)) session->_maxInputChunk  = inPolicy->GetMaxTransferChunkSize(PolicyHolder(session, true));
                     if ((outPolicy)&&(session->_maxOutputChunk > 0)) session->_maxOutputChunk = outPolicy->GetMaxTransferChunkSize(PolicyHolder(session, false));
                  }
               }
            }

            // Now that all is prepared, calculate all the policies' wakeup times
            {
               HashtableIterator<PolicyRef, bool> iter = policies.GetIterator();
               const PolicyRef * next;
               while((next = iter.GetNextKey()) != NULL) CallGetPulseTimeAux(*(next->GetItemPointer()), now, nextPulseAt);
            }
         }
      }

      // This block is the center of the MUSCLE server's universe -- where we wait for the next event, inside select()
      {
         // Calculate timeout value for the next call to Pulse() (if any)
         struct timeval waitTime;
         if (nextPulseAt != MUSCLE_TIME_NEVER) 
         {
            uint64 now = GetCurrentTime64();
            uint64 waitTime64 = (nextPulseAt > now) ? (nextPulseAt - now) : 0;
            Convert64ToTimeVal(waitTime64, waitTime);
         }

         // Block here until the next I/O or pulse event becomes ready
         int numBitsSet = select(maxSocket+1, (maxSocket >= 0) ? &readSet : NULL, (maxSocket >= 0) ? &writeSet : NULL, NULL, (nextPulseAt == MUSCLE_TIME_NEVER) ? NULL : &waitTime);
         if (numBitsSet < 0) 
         {
            if (_doLogging) LogTime(MUSCLE_LOG_CRITICALERROR, "select() failed, aborting!\n");
            ClearLameDucks();
            return B_ERROR;
         }
      }

      // Do the I/O for each of our attached sessions
      {
         AbstractReflectSessionRef * nextRef;
         HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
         while((nextRef = iter.GetNextValue()) != NULL)
         {
            const uint64 now = GetCurrentTime64();
            AbstractReflectSession * session = nextRef->GetItemPointer();
            if (session)
            {
               AbstractMessageIOGateway * gateway = session->GetGateway();
               if (gateway)
               {
                  int socket = GetSocketFor(session);
                  if (socket >= 0)
                  {
                     int32 readBytes = 0, wroteBytes = 0;
                     if (FD_ISSET(socket, &readSet))
                     {
                        AbstractSessionIOPolicy * p = session->GetInputPolicy()();
                        readBytes = gateway->DoInput(session->_maxInputChunk);
                        if ((p)&&(readBytes >= 0)) p->BytesTransferred(PolicyHolder(session, true), (uint32)readBytes);
                     }
                     if (FD_ISSET(socket, &writeSet))
                     {
                        if (session->_connectingAsync) wroteBytes = (FinalizeAsyncConnect(*nextRef) == B_NO_ERROR) ? 0 : -1;
                        else
                        {
                           AbstractSessionIOPolicy * p = session->GetOutputPolicy()();

#ifdef MUSCLE_MAX_OUTPUT_CHUNK
                           // Certain BeOS configurations (i.e. behind routers??) don't handle large outgoing send()'s very
                           // well.  If this is the case with your machine, one easy fix is to put -DMUSCLE_MAX_OUTPUT_CHUNK=1000
                           // or so in your Makefile; that will force all send sizes to be 1000 bytes or less.
                           // (shatty reports the problem with a net_server -> Ethernet -> linksys -> pppoe -> dsl config)
                           if (session->_maxOutputChunk > MUSCLE_MAX_OUTPUT_CHUNK) session->_maxOutputChunk = MUSCLE_MAX_OUTPUT_CHUNK;
#endif
                           wroteBytes = gateway->DoOutput(session->_maxOutputChunk);
                           if ((p)&&(wroteBytes >= 0)) p->BytesTransferred(PolicyHolder(session, false), (uint32)wroteBytes);
                        }
                     }
                     if ((readBytes < 0)||(wroteBytes < 0))
                     {
                        if (session->ClientConnectionClosed()) AddLameDuckSession(*nextRef);
                        else
                        {
                           if (_doLogging) LogTime(MUSCLE_LOG_INFO, "TCP Connection to [%s]@%i was severed.\n", session->GetHostName(), session->GetPort());
                           ShutdownIOFor(session);
                        }
                     }
                     else if (session->_lastByteOutputAt > 0)
                     {
                        // Check for output stalls
                             if (wroteBytes > 0) session->_lastByteOutputAt = now;  // cool, he's streaming out okay
                        else if (now-session->_lastByteOutputAt > gateway->GetOutputStallLimit())
                        {
                           if (_doLogging) LogTime(MUSCLE_LOG_INFO, "TCP Connection to [%s]@%i timed out (output stall).\n", session->GetHostName(), session->GetPort());
                           if (session->ClientConnectionClosed()) AddLameDuckSession(*nextRef);
                                                             else ShutdownIOFor(session); 
                        }
                     }
                  }
               }
            }
         }
      }

      // Pulse() anyone who has it coming
      {
         const uint64 now = GetCurrentTime64();

         // Tell the session policies we're done doing I/O (for now)
         if (policies.GetNumItems() > 0)
         {
            const PolicyRef * next;
            HashtableIterator<PolicyRef, bool> iter = policies.GetIterator();
            while((next = iter.GetNextKey()) != NULL) 
            {
               AbstractSessionIOPolicy * p = next->GetItemPointer();
               if (p->_hasBegun)
               {
                  p->EndIO(now);
                  p->_hasBegun = false;
               }
            }
         }

         // Pulse the Policies
         if (policies.GetNumItems() > 0)
         {
            const PolicyRef * next;
            HashtableIterator<PolicyRef, bool> iter = policies.GetIterator();
            while((next = iter.GetNextKey()) != NULL) CallPulseAux(*(next->GetItemPointer()), now);
         }

         // Pulse the Server
         CallPulseAux(*this, now);

         // Pulse the Sessions and their Gateways
         if (_sessions.GetNumItems() > 0)
         {
            AbstractReflectSessionRef * nextRef;
            HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
            while((nextRef = iter.GetNextValue()) != NULL)
            {
               AbstractReflectSession * session = nextRef->GetItemPointer();
               if (session)
               {
                  CallPulseAux(*session, now); 
                  AbstractMessageIOGateway * gw = session->GetGateway();
                  if (gw) CallPulseAux(*gw, now);
               }
            }
         }

         // Pulse the session-factories
         if (_factories.GetNumItems() > 0)
         {
            HashtableIterator<uint16, ReflectSessionFactoryRef> iter = _factories.GetIterator();
            ReflectSessionFactoryRef * acc;
            while((acc = iter.GetNextValue()) != NULL)
            {
               ReflectSessionFactory * factory = acc->GetItemPointer();
               if (factory) CallPulseAux(*factory, now);
            }
         }
      }
      policies.Clear();

      // Let the sessions handle any messages that were queued up during the I/O phase
      while(true)
      {
         bool loopAgain = false;

         AbstractReflectSessionRef sRef;   // keep a copy here to prevent premature self-deletions
         HashtableIterator<const char *, AbstractReflectSessionRef> iter = GetSessions();
         while((loopAgain == false)&&(iter.GetNextValue(sRef) == B_NO_ERROR))
         {
            AbstractReflectSession * s = sRef();
            if (s)
            {
               // Note:  It *is* necessary to check fromSession->GetGateway() every time through this
               //        loop, because a ReplaceSession() call might set it to NULL.
               MessageRef nextMessageRef;
               if (_watchMemUsage) (void) _watchMemUsage->SetAllocationHasFailed(false);  // (s)'s responsibility for starts here!  If we run out of mem on his watch, he's history
               while((s->GetGateway())&&(s->GetGateway()->GetNextIncomingMessage(nextMessageRef) == B_NO_ERROR)) 
               {
                  s->CallMessageReceivedFromGateway(nextMessageRef);
                  if ((_watchMemUsage)&&(_watchMemUsage->HasAllocationFailed())) break;
                  if (s->GetGateway() == NULL)
                  {
                     // oops!  This session must have replaced itself.  Start through the loop again.
                     loopAgain = true;
                     break;
                  }
               }
               if ((_watchMemUsage)&&(_watchMemUsage->HasAllocationFailed()))
               {
                  _watchMemUsage->SetAllocationHasFailed(false);  // clear the flag
                  if (_doLogging) LogTime(MUSCLE_LOG_CRITICALERROR, "Low Memory!  Aborting active session [%s]@%i to get some back!\n", s->GetHostName(), s->GetPort());
                  AddLameDuckSession(sRef);
                  DumpBoggedSessions();       // see what other cleanup we can do
                  break;
               }
            }
         }

         if (loopAgain == false) break;
      }

      // Lastly, check our accepting ports to see if anyone is trying to connect...
      if (_factories.GetNumItems() > 0)
      {
         HashtableIterator<uint16, ReflectSessionFactoryRef> iter = _factories.GetIterator();
         uint16 port;
         ReflectSessionFactoryRef * acc;
         while((iter.GetNextKey(port) == B_NO_ERROR)&&((acc = iter.GetNextValue()) != NULL)) 
         {
            int acceptSocket = (*acc)()->_socket;
            if (FD_ISSET(acceptSocket, &readSet)) (void) DoAccept(port, acceptSocket, (*acc)());
         }
      }
   }
   (void) ClearLameDucks();  // get rid of any leftover ducks
   return B_NO_ERROR;
}

void ReflectServer :: ShutdownIOFor(AbstractReflectSession * session)
{
   AbstractMessageIOGateway * gw = session->GetGateway();
   if (gw) 
   {
      DataIO * io = gw->GetDataIO();
      if (io) io->Shutdown();  // so we won't try to do I/O on this one anymore
   }
}

status_t ReflectServer :: ClearLameDucks()
{
   // Delete any factories that were previously marked for deletion
   _lameDuckFactories.Clear();

   // Remove any sessions that were previously marked for removal
   AbstractReflectSessionRef duckRef;
   while(_lameDuckSessions.RemoveHead(duckRef) == B_NO_ERROR)
   {
      AbstractReflectSession * duck = duckRef();
      if (duck)
      {
         const char * id = duck->GetSessionIDString();
         if (_sessions.ContainsKey(id))
         {
            duck->AboutToDetachFromServer();
            if (_doLogging) LogTime(MUSCLE_LOG_INFO, "Closed %s [%s] to [%s]@%i (%lu left)\n", duck->GetTypeName(), id, duck->GetHostName(), duck->GetPort(), _sessions.GetNumItems()-1);
            duck->_owner = NULL;
            (void) _sessions.Remove(id);
         }
      }
   }

   return _keepServerGoing ? B_NO_ERROR : B_ERROR;
}

status_t ReflectServer :: DoAccept(uint16 port, int acceptSocket, ReflectSessionFactory * optFactory)
{
   // Accept a new connection and try to start up a session for it
   int newSocket = Accept(acceptSocket);
   if (newSocket >= 0)
   {
      uint32 remoteIP = GetPeerIPAddress(newSocket);
      if (remoteIP > 0)
      {
         String remoteIPString = Inet_NtoA(remoteIP);
         AbstractReflectSessionRef newSessionRef(optFactory ? optFactory->CreateSession(remoteIPString) : NULL, NULL);
         if (newSessionRef())
         {
            newSessionRef()->_port = port;
            if (AddNewSession(newSessionRef, newSocket) == B_NO_ERROR) return B_NO_ERROR;  // success!
         }
         else if ((optFactory)&&(_doLogging)) LogTime(MUSCLE_LOG_INFO, "Session creation denied for [%s] on port %u.\n", remoteIPString(), port);
      }
      else if (_doLogging) LogTime(MUSCLE_LOG_ERROR, "Couldn't get peer IP address for new accept session!\n");

      CloseSocket(newSocket);
   }
   else if (_doLogging) LogTime(MUSCLE_LOG_ERROR, "Accept() failed on port %u (socket %i)\n", port, acceptSocket);

   return B_ERROR;
}

void ReflectServer :: DumpBoggedSessions()
{
   // New for v1.82:  also find anyone whose outgoing message queue is getting too large.
   //                 (where "too large", for now, is more than 5 megabytes)
   // This could happen if someone has a really slow Internet connection, or has decided to
   // stop reading from their end indefinitely for some reason.
   const int MAX_MEGABYTES = 5;
   AbstractReflectSessionRef * lRef;
   HashtableIterator<const char *, AbstractReflectSessionRef> xiter = GetSessions();
   while((lRef = xiter.GetNextValue()) != NULL)
   {
      AbstractReflectSession * asr = lRef->GetItemPointer();
      if (asr)
      {
         AbstractMessageIOGateway * gw = asr->GetGateway();
         if (gw)
         {
            uint32 qSize = 0;
            const Queue<MessageRef> & q = gw->GetOutgoingMessageQueue();
            for (int k=q.GetNumItems()-1; k>=0; k--)
            {
               const Message * qmsg = q[k].GetItemPointer();
               if (qmsg) qSize += qmsg->FlattenedSize();
            }
            if (qSize > MAX_MEGABYTES*1024*1024)
            {
               if (_doLogging) LogTime(MUSCLE_LOG_CRITICALERROR, "Low Memory!  Aborting bogged session [%s]@%i to get some back!\n", asr->GetHostName(), asr->GetPort());
               AddLameDuckSession(*lRef);
            }
         }
      }
   }
}

AbstractReflectSessionRef
ReflectServer ::
GetSession(const char * name) const
{
   AbstractReflectSessionRef ref;
   (void) _sessions.Get(name, ref); 
   return ref;
}

ReflectSessionFactoryRef
ReflectServer ::
GetFactory(uint16 port) const
{
   ReflectSessionFactoryRef ref;
   (void) _factories.Get(port, ref); 
   return ref;
}

status_t
ReflectServer ::
ReplaceSession(AbstractReflectSessionRef newSessionRef, AbstractReflectSession * oldSession)
{
   // move the gateway from the old session to the new one...
   AbstractReflectSession * newSession = newSessionRef.GetItemPointer();
   if (newSession == NULL) return B_ERROR;

   newSession->SetGateway(oldSession->GetGatewayRef());
   newSession->_hostName = oldSession->_hostName;
   newSession->_port     = oldSession->_port;

   if (AddNewSession(newSessionRef) == B_NO_ERROR)
   {
      oldSession->SetGateway(AbstractMessageIOGatewayRef());   /* gateway now belongs to newSession */
      EndSession(oldSession);
      return B_NO_ERROR;
   }
   else
   {
       // Oops, rollback changes and error out
       newSession->SetGateway(AbstractMessageIOGatewayRef());
       newSession->_hostName = "";
       newSession->_port     = 0; 
       return B_ERROR;
   }
}


void
ReflectServer ::
EndSession(AbstractReflectSession * who)
{
   AbstractReflectSessionRef * lRef; 
   HashtableIterator<const char *, AbstractReflectSessionRef> xiter = GetSessions();
   while((lRef = xiter.GetNextValue()) != NULL)
   {
      AbstractReflectSession * session = lRef->GetItemPointer();
      if (session == who)
      {
         AddLameDuckSession(*lRef);
         return;
      }
   }
}

void
ReflectServer ::
EndServer()
{
   _keepServerGoing = false;
}

status_t
ReflectServer ::
PutAcceptFactory(uint16 port, ReflectSessionFactoryRef factoryRef)
{
   (void) RemoveAcceptFactory(port); // Get rid of any previous acceptor on this port...

   ReflectSessionFactory * f = factoryRef();
   if (f)
   {
      int acceptSocket = CreateAcceptingSocket(port, 20, &port);
      if (acceptSocket >= 0)
      {
         if (_factories.Put(port, factoryRef) == B_NO_ERROR)
         {
            f->_owner  = this;
            f->_socket = acceptSocket;
            f->_port   = port;
            if (f->AttachedToServer() == B_NO_ERROR) return B_NO_ERROR;
            else
            {
               (void) RemoveAcceptFactory(port);
               return B_ERROR;
            }
         }
         CloseSocket(acceptSocket);
      }
   }
   return B_ERROR;
}

void
ReflectServer ::
RemoveAcceptFactoryAux(ReflectSessionFactoryRef ref)
{
   ReflectSessionFactory * factory = ref();
   if (factory) 
   {
      factory->AboutToDetachFromServer();
      CloseSocket(factory->_socket);
      factory->_owner  = NULL;
      factory->_port   = 0;
      factory->_socket = -1;
      _lameDuckFactories.AddTail(ref);  // we'll actually have (factory) deleted later on, since at the moment 
   }                                    // we could be in the middle of (factory)'s own method call!
}

status_t
ReflectServer ::
RemoveAcceptFactory(uint16 port)
{
   if (port > 0)
   {
      ReflectSessionFactoryRef acc;
      if (_factories.Get(port, acc) != B_NO_ERROR) return B_ERROR;

      RemoveAcceptFactoryAux(acc);
      (void) _factories.Remove(port);  // do this after the AboutToDetach callback
   }
   else
   {
      HashtableIterator<uint16, ReflectSessionFactoryRef> iter = _factories.GetIterator();
      uint16 nextKey;
      ReflectSessionFactoryRef nextValue;
      while((iter.GetNextKey(nextKey) == B_NO_ERROR)&&(iter.GetNextValue(nextValue) == B_NO_ERROR)) 
      {
         RemoveAcceptFactoryAux(nextValue);
         _factories.Remove(nextKey);  // make sure nobody accesses the factory after it is detached
      }
   }
   return B_NO_ERROR;
}

status_t 
ReflectServer :: 
FinalizeAsyncConnect(AbstractReflectSessionRef ref)
{
   AbstractReflectSession * session = ref();
   if (session)
   {
      session->_connectingAsync = false;  // we're legit now!  :^)
      if (muscle::FinalizeAsyncConnect(GetSocketFor(session)) == B_NO_ERROR)
      {
         session->AsyncConnectCompleted();
         return B_NO_ERROR;
      }
   }
   return B_ERROR;
}

uint64
ReflectServer ::
GetServerUptime() const
{
   return (_startedAt > 0) ? (GetCurrentTime64() - _startedAt) : 0;
}
                        
uint32 
ReflectServer ::
GetNumAvailableBytes() const
{
   uint32 alloced = GetNumUsedBytes();
   uint32 max     = GetMaxNumBytes();
   return (alloced < max) ? (max - alloced) : 0;
}
 
uint32 
ReflectServer ::
GetMaxNumBytes() const
{
   return _watchMemUsage ? (uint32)_watchMemUsage->GetMaxNumBytes() : MUSCLE_NO_LIMIT;
}
 
uint32 
ReflectServer ::
GetNumUsedBytes() const
{
#ifdef MUSCLE_ENABLE_MEMORY_TRACKING
   return (uint32) GetNumAllocatedBytes();
#else
   return 0;  // if we're not tracking, there is no way to know!
#endif
}

void
ReflectServer :: AddLameDuckSession(AbstractReflectSessionRef ref)
{
   if ((_lameDuckSessions.IndexOf(ref) < 0)&&(_lameDuckSessions.AddTail(ref) != B_NO_ERROR)&&(_doLogging)) LogTime(MUSCLE_LOG_CRITICALERROR, "Server:  AddLameDuckSession() failed, I'm REALLY in trouble!  Aggh!\n");
}

int
ReflectServer :: GetSocketFor(AbstractReflectSession * session) const
{
   AbstractMessageIOGateway * gw = session->GetGateway();
   if (gw)
   {
      DataIO * io = gw->GetDataIO();
      if (io) return io->GetSelectSocket();
   }
   return -1;         
}


};  // end namespace muscle
