/* This file is Copyright 2000-2008 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */

#ifndef MuscleServerComponent_h
#define MuscleServerComponent_h

#include "message/Message.h"
#include "util/RefCount.h"
#include "util/PulseNode.h"
#include "util/NetworkUtilityFunctions.h"  // for ip_address

BEGIN_NAMESPACE(muscle);

class AbstractReflectSession;
class ReflectServer;
class ReflectSessionFactory;

/** Type declaration for a reference-count-pointer to an AbstractReflectSession */
typedef Ref<AbstractReflectSession> AbstractReflectSessionRef;

/** Type declaration for a reference-count-pointer to a ReflectSessionFactory */
typedef Ref<ReflectSessionFactory> ReflectSessionFactoryRef;   

/** 
 *  This class represents any object that can be added to a ReflectServer object
 *  in one way or another, to help define the ReflectServer's behaviour.  This
 *  class provides callback wrappers that let you operate on the server's state.
 */
class ServerComponent : public RefCountable, public PulseNode
{
public:
   /** Default Constructor. */
   ServerComponent();

   /** Destructor. */
   virtual ~ServerComponent();

   /**
    * This method is called when this object has been added to
    * a ReflectServer object.  When this method is called, it
    * is okay to call the other methods in the ServerComponent API.  
    * Should return B_NO_ERROR if everything is okay; something
    * else if there is a problem and the attachment should be aborted.
    * Default implementation does nothing and returns B_NO_ERROR.
    * If you override this, be sure to call your superclass's implementation
    * of this method as the first thing in your implementation, and
    * if it doesn't return B_NO_ERROR, immediately return an error yourself.
    */
   virtual status_t AttachedToServer();

   /**
    * This method is called just before we are removed from the
    * ReflectServer object.  Methods in the ServerComponent API may
    * still be called at this time (but not after this method returns).
    * Default implementation does nothing.
    * If you override this, be sure to call you superclass's implementation
    * of this method as the last thing you do in your implementation.
    */
   virtual void AboutToDetachFromServer();

   /**
    * Called when a message is sent to us by an AbstractReflectSession object.
    * Default implementation is a no-op.
    * @param from The session who is sending the Message to us.
    * @param msg A reference to the message that was sent.
    * @param userData Additional data whose semantics are determined by the sending subclass.
    *                 (For StorageReflectSessions, this value, if non-NULL, is a pointer to the
    *                  DataNode in this Session's node subtree that was matched by the paths in (msg))
    */
   virtual void MessageReceivedFromSession(AbstractReflectSession & from, const MessageRef & msg, void * userData);

   /**
    * Called when a message is sent to us by a ReflectSessionFactory object.
    * Default implementation is a no-op.
    * @param from The session who is sending the Message to us.
    * @param msg A reference to the message that was sent.
    * @param userData Additional data whose semantics are determined by the sending subclass.
    */
   virtual void MessageReceivedFromFactory(ReflectSessionFactory & from, const MessageRef & msg, void * userData);

   /** Returns true if we are attached to the ReflectServer object, false if we are not.  */
   bool IsAttachedToServer() const {return (_owner != NULL);}

   /** Returns the ReflectServer we are currently attached to, or NULL if we aren't currently attached to a ReflectServer. */
   ReflectServer * GetOwner() const {return _owner;}

   /** Sets the ReflectServer we are currently attached to.  Don't call this if you don't know what you are doing. */
   void SetOwner(ReflectServer * s) {_owner = s;}

protected:
   /** Returns the number of milliseconds that the server has been running. */
   uint64 GetServerUptime() const;

   /** Returns the number of bytes that are currently available to be allocated */
   uint32 GetNumAvailableBytes() const;
 
   /** Returns the maximum number of bytes that may be allocated at any given time */
   uint32 GetMaxNumBytes() const;
 
   /** Returns the number of bytes that are currently allocated */
   uint32 GetNumUsedBytes() const;

   /** Passes through to ReflectServer::PutAcceptFactory() */
   status_t PutAcceptFactory(uint16 port, const ReflectSessionFactoryRef & factoryRef, const ip_address & interfaceIP = invalidIP, uint16 * optRetPort = NULL);

   /** Passes through to ReflectServer::RemoveAcceptFactory() */
   status_t RemoveAcceptFactory(uint16 port, const ip_address & interfaceIP = invalidIP);

   /** Tells the whole server process to quit ASAP.  */
   void EndServer();

   /**
    * Returns a reference to a Message that is shared by all objects in
    * a single ReflectServer.  This message can be used for whatever 
    * purpose the ServerComponents care to; it is not used by the 
    * server itself.  (Note that StorageReflectSessions add data to
    * this Message and expect it to remain there, so be careful not
    * to remove or overwrite it if you are using StorageReflectSessions)
    */
   Message & GetCentralState() const;

   /**
    * Adds the given AbstractReflectSession to the server's session list.
    * If (socket) is less than zero, no TCP connection will be used,
    * and the session will be a pure server-side entity.
    * @param session A reference to the new session to add to the server.
    * @param socket the socket descriptor associated with the new session, or a NULL reference.
    *               If the socket descriptor argument is a NULL reference, the session's
    *               CreateDefaultSocket() method will be called to supply the SocketRef.
    *               If that also returns a NULL reference, then the client will run without
    *               a connection to anything.
    * @return B_NO_ERROR if the session was successfully added, or B_ERROR on error.
    */
   status_t AddNewSession(const AbstractReflectSessionRef & session, const SocketRef & socket = SocketRef());

   /**
    * Like AddNewSession(), only creates a session that connects asynchronously to
    * the given IP address.  AttachedToServer() will be called immediately on the
    * session, and then when the connection is complete, AsyncConnectCompleted() will
    * be called.  Other than that, however, (session) will behave like any other session,
    * except any I/O messages for the client won't be transferred until the connection
    * completes.
    * @param session A reference to the new session to add to the server.
    * @param targetIPAddress IP address to connect to
    * @param port port to connect to at that address
    * @param autoReconnectDelay If specified, this is the number of microseconds after the
    *                           connection is broken that an automatic reconnect should be
    *                           attempted.  If not specified, an automatic reconnect will not
    *                           be attempted, and the session will be removed when the
    *                           connection breaks.  Specifying this is equivalent to calling
    *                           SetAutoReconnectDelay() on (session).
    * @return B_NO_ERROR if the session was successfully added, or B_ERROR on error 
    *                    (out-of-memory or the connect attempt failed immediately).
    */
   status_t AddNewConnectSession(const AbstractReflectSessionRef & session, const ip_address & targetIPAddress, uint16 port, uint64 autoReconnectDelay = MUSCLE_TIME_NEVER);

   /**
    * Like AddNewConnectSession(), except that the added session will not initiate
    * a TCP connection to the specified address immediately.  Instead, it will just
    * hang out and do nothing until you call Reconnect() on it.  Only then will it
    * create the TCP connection to the address specified here.
    * @param ref New session to add to the server.
    * @param targetIPAddress IP address to connect to
    * @param port Port to connect to at that IP address.
    * @param autoReconnectDelay If specified, this is the number of microseconds after the
    *                           connection is broken that an automatic reconnect should be
    *                           attempted.  If not specified, an automatic reconnect will not
    *                           be attempted, and the session will be removed when the
    *                           connection breaks.  Specifying this is equivalent to calling
    *                           SetAutoReconnectDelay() on (ref).
    * @return B_NO_ERROR if the session was successfully added, or B_ERROR on error
    *                    (out-of-memory, or the connect attempt failed immediately)
    */
   status_t AddNewDormantConnectSession(const AbstractReflectSessionRef & ref, const ip_address & targetIPAddress, uint16 port, uint64 autoReconnectDelay = MUSCLE_TIME_NEVER);

   /** Returns our server's table of attached sessions. */
   const Hashtable<const String *, AbstractReflectSessionRef> & GetSessions() const;

   /**
    * Looks up a session connected to our ReflectServer via its session ID string.
    * @param id The ID of the session you are looking for.
    * @return A reference to the session with the given session ID, or a NULL reference on failure.
    */
   AbstractReflectSessionRef GetSession(uint32 id) const;
   
   /**
    * Looks up a session connected to our ReflectServer via its session ID string.
    * @param id The ID string of the session you are looking for.
    * @return A reference to the session with the given session ID, or a NULL reference on failure.
    */
   AbstractReflectSessionRef GetSession(const String & id) const;
   
   /** Returns the table of session factories currently attached to the server. */
   const Hashtable<IPAddressAndPort, ReflectSessionFactoryRef> & GetFactories() const;

   /** Given a port number, returns a reference to the factory of that port, or a NULL reference if no
such factory exists. */
   ReflectSessionFactoryRef GetFactory(uint16) const;         

private:
   ReflectServer * _owner;
};

END_NAMESPACE(muscle);

#endif
