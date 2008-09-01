/* This file is Copyright 2000-2008 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */

#ifndef MuscleAbstractReflectSession_h
#define MuscleAbstractReflectSession_h

#include "iogateway/AbstractMessageIOGateway.h"
#include "reflector/AbstractSessionIOPolicy.h"
#include "reflector/ServerComponent.h"
#include "util/Queue.h"
#include "util/RefCount.h"

BEGIN_NAMESPACE(muscle);

/** This is an interface for an object that knows how to create new
 *  AbstractReflectSession objects when needed.  It is used by the
 *  ReflectServer classes to generate sessions when connections are received.
 */
class ReflectSessionFactory : public ServerComponent
{
public:
   /** Constructor.  Our globally unique factory ID is assigned here. */
   ReflectSessionFactory();

   /** Destructor */
   virtual ~ReflectSessionFactory() {/* empty */}

   /** Should be overriden to return a new ReflectSession object, or NULL on failure.
    *  @param clientAddress A string representing the connecting client's host (typically an IP address, e.g. "192.168.1.102")
     * @param factoryInfo The IP address and port number of the local network interface on which this connection was received.
    *  @returns a reference to a freshly allocated AbstractReflectSession object on success, or a NULL reference on failure.
    */
   virtual AbstractReflectSessionRef CreateSession(const String & clientAddress, const IPAddressAndPort & factoryInfo) = 0;

   /**
    * Should return true iff this factory is currently ready to accept connections.
    * This method is called each time through the event loop, and if this method
    * is overridden to return false, then this factory will not be included in the
    * select() set and any incoming TCP connections on this factory's port will
    * not be acted upon (i.e. they will be forced to wait).
    * Default implementation always returns true.
    */
   virtual bool IsReadyToAcceptSessions() const {return true;}

   /**
    * Returns an auto-assigned ID value that represents this factory.
    * The returned value is guaranteed to be unique across all factory in the server.
    */
   uint32 GetFactoryID() const {return _id;}

protected:
   /**
    * Convenience method:  Calls MessageReceivedFromFactory() on all session
    * objects.  Saves you from having to do your own iteration every time you
    * want to broadcast something.
    * @param msgRef a reference to the Message you wish to broadcast
    * @param userData any userData value you care to include.  Defaults to NULL.
    */
   void BroadcastToAllSessions(const MessageRef & msgRef, void * userData = NULL);

   /**
    * Convenience method:  Calls MessageReceivedFromFactory() on all session-factory
    * objects.  Saves you from having to do your own iteration every time you
    * want to broadcast something to the factories.
    * @param msgRef a reference to the Message you wish to broadcast
    * @param userData any userData value you care to include.  Defaults to NULL.
    * @param includeSelf Whether or not MessageReceivedFromFactory() should be called on 'this' factory.  Defaults to true.
    */
   void BroadcastToAllFactories(const MessageRef & msgRef, void * userData = NULL, bool includeSelf = true);

private:
   uint32 _id;
};

/** This is a partially specialized factory that knows how to act as a facade for a "slave" factory.
  * In particular, it contains implementations of AttachedToServer() and AboutToDetachFromServer()
  * that set up and tear down the slave factory appropriately.  This way that logic doesn't have
  * to be replicated in every ReflectSessionFactory subclass that wants to use the facade pattern.
  */
class ProxySessionFactory : public ReflectSessionFactory
{
public:
   ProxySessionFactory(const ReflectSessionFactoryRef & slaveRef) : _slaveRef(slaveRef) {/* empty */}
   virtual ~ProxySessionFactory() {/* empty */}

   virtual status_t AttachedToServer();
   virtual void AboutToDetachFromServer();

   /** Returns the reference to the "slave" factory that was passed in to our constructor. */
   const ReflectSessionFactoryRef & GetSlave() const {return _slaveRef;}

private:
   ReflectSessionFactoryRef _slaveRef;
};

/** This is the abstract base class that defines the server side logic for a single
 *  client-server connection.  This class contains no message routing logic of its own,
 *  but defines the interface so that subclasses can do so.
 */
class AbstractReflectSession : public ServerComponent, public AbstractGatewayMessageReceiver
{
public:
   /** Default Constructor. */
   AbstractReflectSession();

   /** Destructor. */
   virtual ~AbstractReflectSession();

   /** Returns the hostname of this client that is associated with this session.
     * May only be called if this session is currently attached to a ReflectServer.
     */
   const String & GetHostName() const;

   /** Returns the server-side port that this session was accepted on, or 0 if
     * we weren't accepted from a port (e.g. we were created locally)
     * May only be called if this session is currently attached to a ReflectServer.
     */
   uint16 GetPort() const;

   /** Returns the server-side network interface IP that this session was accepted on,
     * or 0 if we weren't created via accepting a network connection  (e.g. we were created locally)
     * May only be called if this session is currently attached to a ReflectServer.
     */
   const ip_address & GetLocalInterfaceAddress() const;

   /** Returns a globally unique ID for this session. */
   uint32 GetSessionID() const {return _sessionID;}

   /**
    * Returns an ID string to represent this session with.
    * (This string is the ASCII representation of GetSessionID())
    */
   const String & GetSessionIDString() const {return _idString;}

   /** Marks this session for immediate termination and removal from the server. */
   virtual void EndSession();

   /** Forces the disconnection of this session's TCP connection to its client.
    *  Calling this will cause ClientConnectionClosed() to be called, as if the
    *  TCP connection had been severed externally.
    *  @returns the value that ClientConnectionClosed() returned.
    */
   bool DisconnectSession();

   /**
    * Causes this session to be terminated (similar to EndSession(),
    * and the session specified in (newSessionRef) to take its
    * place using the same socket connection & message IO gateway.
    * @param newSession the new session object that is to take the place of this one.
    * @return B_NO_ERROR on success, B_ERROR if the new session refused to be attached.
    */
   status_t ReplaceSession(const AbstractReflectSessionRef & newSession);

   /**
    * Called when the TCP connection to our client is broken.
    * If this method returns true, then this session will be removed and
    * deleted.
    * @return If it returns false, then this session will continue, even
    *         though the client is no longer available.  Default implementation
    *         always returns true, unless the automatic-reconnect feature has
    *         been enabled (via SetAutoReconnectDelay()), in which case this
    *         method will return false and try to Reconnect() again, instead.
    */
   virtual bool ClientConnectionClosed();

   /**
    * For sessions that were added to the server with AddNewConnectSession(),
    * or AddNewDormantConnectSession(), this method is called when the asynchronous
    * connect process completes successfully.  (if the asynchronous connect fails,
    * ClientConnectionClosed() is called instead).  Default implementation just sets
    * an internal flag that governs whether an error message should be printed when
    * the session is disconnected later on.
    */
   virtual void AsyncConnectCompleted();

   /**
    * Set a new input I/O policy for this session.
    * @param newPolicy Reference to the new policy to use to control the incoming byte stream
    *                  for this session.  May be a NULL reference if you just want to remove the existing policy.
    */
   void SetInputPolicy(const PolicyRef & newPolicy);

   /** Returns a reference to the current input policy for this session.
     * May be a NULL reference, if there is no input policy installed (which is the default state)
     */
   PolicyRef GetInputPolicy() const {return _inputPolicyRef;}

   /**
    * Set a new output I/O policy for this session.
    * @param newPolicy Reference to the new policy to use to control the outgoing byte stream
    *                 for this session.  May be a NULL reference if you just want to remove the existing policy.
    */
   void SetOutputPolicy(const PolicyRef & newPolicy);

   /** Returns a reference to the current output policy for this session.  May be a NULL reference.
     * May be a NULL reference, if there is no output policy installed (which is the default state)
     */
   PolicyRef GetOutputPolicy() const {return _outputPolicyRef;}

   /** Installs the given AbstractMessageIOGateway as the gateway we should use for I/O.
     * If this method isn't called, the ReflectServer will call our CreateGateway() method
     * to set our gateway for us when we are attached.
     * @param ref Reference to the I/O gateway to use, or a NULL reference to remove any gateway we have.
     */
   void SetGateway(const AbstractMessageIOGatewayRef & ref) {_gateway = ref; _outputStallLimit = _gateway()?_gateway()->GetOutputStallLimit():MUSCLE_TIME_NEVER;}

   /**
    * Returns a reference to our internally held message IO gateway object,
    * or NULL reference if there is none.  The returned gateway remains
    * the property of this session.
    */
   const AbstractMessageIOGatewayRef & GetGateway() const {return _gateway;}

   /** Should return true iff we have data pending for output.
    *  Default implementation calls HasBytesToOutput() on our installed AbstractDataIOGateway object,
    *  if we have one, or returns false if we don't.
    */
   virtual bool HasBytesToOutput() const;

   /**
     * Should return true iff we are willing to read more bytes from our
     * client connection at this time.  Default implementation calls
     * IsReadyForInput() on our install AbstractDataIOGateway object, if we
     * have one, or returns false if we don't.
     *
     */
   virtual bool IsReadyForInput() const;

   /** Called by the ReflectServer when it wants us to read some more bytes from our client.
     * Default implementation simply calls DoInput() on our Gateway object (if any).
     * @param receiver Object to call CallMessageReceivedFromGateway() on when new Messages are ready to be looked at.
     * @param maxBytes Maximum number of bytes to read before returning.
     * @returns The total number of bytes read, or -1 if there was a fatal error.
     */
   virtual int32 DoInput(AbstractGatewayMessageReceiver & receiver, uint32 maxBytes);

   /** Called by the ReflectServer when it wants us to push some more bytes out to our client.
     * Default implementation simply calls DoOutput() on our Gateway object (if any).
     * @param maxBytes Maximum number of bytes to write before returning.
     * @returns The total number of bytes written, or -1 if there was a fatal error.
     */
   virtual int32 DoOutput(uint32 maxBytes);

   /** Socket factory method.  This method is called by AddNewSession() when
    *  no valid Socket was supplied as an argument to the AddNewSession() call.
    *  This method should either create and supply a default Socket, or return
    *  a NULL SocketRef.  In the latter case, the session will run without any
    *  connection to a client.
    *  Default implementation always returns a NULL SocketRef.
    */
   virtual SocketRef CreateDefaultSocket();

   /** DataIO factory method.  Should return a new non-blocking DataIO
    *  object for our gateway to use, or NULL on failure.  Called by
    *  ReflectServer when this session is added to the server.
    *  The default implementation returns a non-blocking TCPSocketDataIO
    *  object, which is the correct behaviour 99% of the time.
    *  @param socket The socket to provide the DataIO object for.
    *                On success, the DataIO object becomes owner of (socket).
    *  @return A newly allocated DataIO object, or NULL on failure.
    */
   virtual DataIORef CreateDataIO(const SocketRef & socket);

   /**
    * Gateway factory method.  Should return a reference to a new
    * AbstractMessageIOGateway for this session to use for communicating
    * with its remote peer.
    * Called by ReflectServer when this session object is added to the
    * server, but doesn't already have a valid gateway installed.
    * The default implementation returns a MessageIOGateway object.
    * @return a new message IO gateway object, or a NULL reference on failure.
    */
   virtual AbstractMessageIOGatewayRef CreateGateway();

   /** Overridden to support auto-reconnect via SetAutoReconnectDelay() */
   virtual uint64 GetPulseTime(uint64 now, uint64 sched);

   /** Overridden to support auto-reconnect via SetAutoReconnectDelay() */
   virtual void Pulse(uint64 now, uint64 sched);

   /** Should return a pretty, human readable string identifying this class.  */
   virtual const char * GetTypeName() const = 0;

   /** May be overridden to return the host name string we should be assigned
    *  if no host name could be automatically determined by the ReflectServer
    *  (i.e. if we had no associated socket at the time).
    *  Default implementation returns "<unknown>".
    */
   virtual String GetDefaultHostName() const;

   /** Convenience method -- returns a human-readable string describing our
    *  type, our hostname, our session ID, and what port we are connected to.
    */
   String GetSessionDescriptionString() const;

   /** Returns the IP address we connected asynchronously to.
    *  The returned value is meaningful only if we were added
    *  with AddNewConnectSession() or AddNewDormantConnectSession().
    */
   const ip_address & GetAsyncConnectIP() const {return _asyncConnectDest.GetIPAddress();}

   /** Returns the remote port we connected asynchronously to.
    *  The returned value is meaningful only if we were added
    *  with AddNewConnectSession() or AddNewDormantConnectSession().
    */
   uint16 GetAsyncConnectPort() const {return _asyncConnectDest.GetPort();}

   /** Returns the node path of the node representing this session (e.g. "/192.168.1.105/17") */
   virtual const String & GetSessionRootPath() const {return _sessionRootPath;}

   /** Sets the amount of time that should pass between when this session loses its connection
     * (that was previously set up using AddNewConnectSession() or AddNewDormantConnectSession())
     * and when it should automatically try to reconnect to that same destination (by calling Reconnect()).
     * Default setting is MUSCLE_TIME_NEVER, meaning that automatic reconnection is disabled.
     * @param delay The amount of time to delay before reconnecting, in microseconds.
     */
   void SetAutoReconnectDelay(uint64 delay) {_autoReconnectDelay = delay; InvalidatePulseTime();}

   /** Returns the current automatic-reconnect delay period, as was previously set by
     * SetAutoReconnectDelay().  Note that this setting is only relevant for sessions
     * that were attached using AddNewConnectSession() or AddNewDormantConnectSession().
     */
   uint64 GetAutoReconnectDelay() const {return _autoReconnectDelay;}

   /** Returns true iff we are currently in the middle of an asynchronous TCP connection */
   bool IsConnectingAsync() const {return _connectingAsync;}

   /** Returns true iff this session is currently connected to our remote counterpart. */
   bool IsConnected() const {return _isConnected;}

protected:
   /**
    * Adds a MessageRef to our gateway's outgoing message queue.
    * (ref) will be sent back to our client when time permits.
    * @param ref Reference to a Message to send to our client.
    * @return B_NO_ERROR on success, B_ERROR if out-of-memory.
    */
   virtual status_t AddOutgoingMessage(const MessageRef & ref);

   /**
    * Convenience method:  Calls MessageReceivedFromSession() on all session
    * objects.  Saves you from having to do your own iteration every time you
    * want to broadcast something.
    * @param msgRef a reference to the Message you wish to broadcast
    * @param userData any userData value you care to include.  Defaults to NULL.
    * @param includeSelf Whether or not MessageReceivedFromSession() should be called on 'this' session.  Defaults to true.
    * @see GetSessions(), AddNewSession(), GetSession()
    */
   void BroadcastToAllSessions(const MessageRef & msgRef, void * userData = NULL, bool includeSelf = true);

   /**
    * Convenience method:  Calls MessageReceivedFromSession() on all installed
    * session-factory objects.  Saves you from having to do your own iteration
    * every time you want to broadcast something to the factories.
    * @param msgRef a reference to the Message you wish to broadcast
    * @param userData any userData value you care to include.  Defaults to NULL.
    * @see GetFactories(), PutFactory(), GetFactory()
    */
   void BroadcastToAllFactories(const MessageRef & msgRef, void * userData = NULL);

   /**
    * Closes this session's current TCP connection (if any), and creates a new
    * TCP socket that will then try to asynchronously connect back to the previous
    * socket's host and port.  Note that this method is primarily useful for sessions
    * that were added with AddNewConnectSession() or AddNewDormantConnectSession();
    * for other types of session, it will just destroy this session's DataIO and IOGateway
    * and then create new ones by calling CreateDefaultSocket() and CreateDataIO().
    * @note This method will call CreateDataIO() to make a new DataIO object for the newly created socket.
    * @returns B_NO_ERROR on success, or B_ERROR on failure.
    *          On success, the connection result will be reported back
    *          later, either via a call to AsyncConnectCompleted() (if the connection
    *          succeeds) or a call to ClientConnectionClosed() (if the connection fails)
    */
   status_t Reconnect();

   /** Convenience method:  Returns the file descriptor associated with this session's
     * DataIO class, or a NULL reference if there is none.
     */
   const SocketRef & GetSessionSelectSocket() const;

   /** Set by StorageReflectSession::AttachedToServer() */
   void SetSessionRootPath(const String & p) {_sessionRootPath = p;}

private:
   void SetPolicyAux(PolicyRef & setRef, uint32 & setChunk, const PolicyRef & newRef, bool isInput);
   void PlanForReconnect();

   friend class ReflectServer;
   uint32 _sessionID;
   String _idString;
   IPAddressAndPort _ipAddressAndPort;
   bool _connectingAsync;
   bool _isConnected;
   String _hostName;
   IPAddressAndPort _asyncConnectDest;
   AbstractMessageIOGatewayRef _gateway;
   uint64 _lastByteOutputAt;
   PolicyRef _inputPolicyRef;
   PolicyRef _outputPolicyRef;
   uint32 _maxInputChunk;   // as determined by our Policy object
   uint32 _maxOutputChunk;  // and stored here for convenience
   uint64 _outputStallLimit;
   bool _scratchReconnected; // scratch, watched by ReflectServer() during ClientConnectionClosed() calls.
   String _sessionRootPath;

   // auto-reconnect support
   uint64 _autoReconnectDelay;
   uint64 _reconnectTime;
   bool _wasConnected;
};

// VC++ (previous to .net) can't handle partial template specialization, so for them we define this explicitly.
#ifdef MUSCLE_USING_OLD_MICROSOFT_COMPILER
DECLARE_HASHTABLE_KEY_CLASS(Ref<AbstractReflectSession>);
#endif

END_NAMESPACE(muscle);

#endif
