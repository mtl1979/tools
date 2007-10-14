/* This file is Copyright 2007 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */

#ifndef MuscleNetworkUtilityFunctions_h
#define MuscleNetworkUtilityFunctions_h

#include "support/MuscleSupport.h"
#include "util/Queue.h"
#include "util/Socket.h"
#include "util/String.h"
#include "util/TimeUtilityFunctions.h"

// These includes are here so that people can use select() without having to #include the proper
// things themselves all the time.
#ifndef WIN32
# include <sys/types.h>
# include <sys/socket.h>
#endif

#ifdef BONE
# include <sys/select.h>  // sikosis at bebits.com says this is necessary... hmm.
#endif

BEGIN_NAMESPACE(muscle);

#ifdef MUSCLE_USE_IPV6
/* IPv6 addressing support */

/** This class represents an IPv6 network address -- it acts like a uint128, 
  * except that only the functions necessary for networking are implemented.
  */
class ip_address
{
public:
   ip_address(uint64 lowBits = 0, uint64 highBits = 0) : _lowBits(lowBits), _highBits(highBits) {/* empty */}
   ip_address(const ip_address & rhs) : _lowBits(rhs._lowBits), _highBits(rhs._highBits) {/* empty */}

   ip_address & operator = (const ip_address & rhs) {_lowBits = rhs._lowBits; _highBits = rhs._highBits; return *this;}

   bool operator == (const ip_address & rhs) const {return ((_lowBits == rhs._lowBits)&&(_highBits == rhs._highBits));}
   bool operator != (const ip_address & rhs) const {return ((_lowBits != rhs._lowBits)||(_highBits != rhs._highBits));}

   bool operator <  (const ip_address & rhs) const {return ((_highBits < rhs._highBits)||((_highBits == rhs._highBits)&&(_lowBits < rhs._lowBits)));}
   bool operator >  (const ip_address & rhs) const {return ((_highBits > rhs._highBits)||((_highBits == rhs._highBits)&&(_lowBits > rhs._lowBits)));}
   bool operator <= (const ip_address & rhs) const {return (*this == rhs)||(*this < rhs);}
   bool operator >= (const ip_address & rhs) const {return (*this == rhs)||(*this > rhs);}

   ip_address operator & (const ip_address & rhs) {return ip_address(_lowBits & rhs._lowBits, _highBits & rhs._highBits);}
   ip_address operator | (const ip_address & rhs) {return ip_address(_lowBits | rhs._lowBits, _highBits | rhs._highBits);}
   ip_address operator ~ () {return ip_address(~_lowBits, ~_highBits);}

   ip_address operator &= (const ip_address & rhs) {*this = *this & rhs; return *this;}
   ip_address operator |= (const ip_address & rhs) {*this = *this | rhs; return *this;}

   void SetBits(uint64 lowBits, uint64 highBits) {_lowBits = lowBits; _highBits = highBits;}

   uint64 GetLowBits() const {return _lowBits;}
   uint64 GetHighBits() const {return _highBits;}

   uint32 HashCode() const {return (uint32)((_lowBits+_highBits)%((uint32)-1));}

   /** Writes our address into the specified uint8 array, in the required network-friendly order.
     * @param networkBuf The 16-byte network-array to write to.  Typically you would pass in 
     *                    mySockAddr_in6.sin6_addr.s6_addr as the argument to this function.
     */
   void WriteToNetworkArray(uint8 * networkBuf) const
   {
      WriteToNetworkArrayAux(&networkBuf[0], _highBits);
      WriteToNetworkArrayAux(&networkBuf[8], _lowBits);
   }

   /** Reads our address in from the specified uint8 array, in the required network-friendly order.
     * @param networkBuf The 16-byte network-array to read from.  Typically you would pass in 
     *                    mySockAddr_in6.sin6_addr.s6_addr as the argument to this function.
     */
   void ReadFromNetworkArray(const uint8 * networkBuf)
   {
      ReadFromNetworkArrayAux(&networkBuf[0], _highBits);
      ReadFromNetworkArrayAux(&networkBuf[8], _lowBits);
   }

private:
   void WriteToNetworkArrayAux( uint8 * out, const uint64 & in ) const {uint64 tmp = B_HOST_TO_BENDIAN_INT64(in); muscleCopyOut(out, tmp);}
   void ReadFromNetworkArrayAux(const uint8 * in, uint64 & out) const {uint64 tmp; muscleCopyIn(tmp, in); out = B_BENDIAN_TO_HOST_INT64(tmp);}

   uint64 _lowBits;
   uint64 _highBits;
};

/** IPv6 Numeric representation of a all-zeroes invalid/guard address */
const ip_address invalidIP(0x00);

/** IPv6 Numeric representation of localhost (::1) for convenience */
const ip_address localhostIP(0x01);

/** IPv6 Numeric representation of link-local broadcast (ff02::1), for convenience */
const ip_address broadcastIP(0x01, ((uint64)0xFF02)<<48);

template <class T> class HashFunctor;

template <>
class HashFunctor<ip_address>
{
public:
   uint32 operator () (const ip_address & x) const {return x.HashCode();}
};

#else

/** IPv4 addressing support */

typedef uint32 ip_address;

/** IPv4 Numeric representation of a all-zeroes invalid/guard address */
const ip_address invalidIP = 0;

/** IPv4 Numeric representation of localhost (127.0.0.1), for convenience */
const ip_address localhostIP = ((((uint32)127)<<24)|((uint32)1));

/** IPv4 Numeric representation of broadcast (255.255.255.255), for convenience */
const ip_address broadcastIP = ((uint32)-1);

#endif

/** Given a hostname or IP address (e.g. "mycomputer.be.com" or "192.168.0.1"),
  * performs a hostname lookup and returns the 4-byte IP address that corresponds
  * with that name.
  * @param name ASCII IP address or hostname to look up.
  * @param expandLocalhost If true, then if (name) corresponds to 127.0.0.1, this function
  *                        will attempt to determine the host machine's actual primary IP
  *                        address and return that instead.  Otherwise, 127.0.0.1 will be
  *                        returned in this case.  Defaults to false.
  * @return The 4-byte IP address (local endianness), or 0 on failure.
  */
ip_address GetHostByName(const char * name, bool expandLocalhost = false);

/** Convenience function for connecting with TCP to a given hostName/port.
 * @param hostName The ASCII host name or ASCII IP address of the computer to connect to.
 * @param port The port number to connect to.
 * @param debugTitle If non-NULL, debug output to stdout will be enabled and debug output will be prefaced by this string.
 * @param debugOutputOnErrorsOnly if true, debug output will be printed only if an error condition occurs.
 * @param maxConnectTime The maximum number of microseconds to spend attempting to make this connection.  If left as MUSCLE_TIME_NEVER
 *                       (the default) then no particular time limit will be enforced, and it will be left up to the operating system
 *                       to decide when the attempt should time out.
 * @param expandLocalhost If true, then if (name) corresponds to 127.0.0.1, this function
 *                        will attempt to determine the host machine's actual primary IP
 *                        address and return that instead.  Otherwise, 127.0.0.1 will be
 *                        returned in this case.  Defaults to false.
 * @return the non-negative sockfd if the connection is successful, -1 if the connection attempt failed.
 */
SocketRef Connect(const char * hostName, uint16 port, const char * debugTitle = NULL, bool debugOutputOnErrorsOnly = true, uint64 maxConnectTime = MUSCLE_TIME_NEVER, bool expandLocalhost = false);

/** Mostly as above, only with the target IP address specified numerically, rather than as an ASCII string. 
 *  This version of connect will never do a DNS lookup.
 * @param hostIP The numeric host IP address to connect to.
 * @param port The port number to connect to.
 * @param debugHostName If non-NULL, we'll print this host name out when reporting errors.  It isn't used for networking purposes, though.
 * @param debugTitle If non-NULL, debug output to stdout will be enabled and debug output will be prefaced by this string.
 * @param debugOutputOnErrorsOnly if true, debug output will be printed only if an error condition occurs.
 * @param maxConnectTime The maximum number of microseconds to spend attempting to make this connection.  If left as MUSCLE_TIME_NEVER
 *                       (the default) then no particular time limit will be enforced, and it will be left up to the operating system
 *                       to decide when the attempt should time out.
 * @return the non-negative sockfd if the connection is successful, -1 if the connection attempt failed.
 */
SocketRef Connect(const ip_address & hostIP, uint16 port, const char * debugHostName = NULL, const char * debugTitle = NULL, bool debugOutputOnErrorsOnly = true, uint64 maxConnectTime = MUSCLE_TIME_NEVER);

/** Convenience function for accepting a TCP connection on a given socket.  Has the same semantics as accept().
 *  (This is somewhat better than calling accept() directly, as certain cross-platform issues get transparently taken care of)
 * @param socket socket FD to accept from (e.g. a socket that was previously returned by CreateAcceptingSocket())
 * @param optRetLocalInfo if non-NULL, then on success, some information about the local side of the new
 *                        connection will be written here.  In particular, optRetLocalInfo()->GetIPAddress()
 *                        will return the IP address of the local network interface that the connection was
 *                        received on, and optRetLocalInfo()->GetPort() will return the port number that
 *                        the connection was received on.  Defaults to NULL.
 * @return the non-negative sockfd if the accept is successful, or -1 if the accept attempt failed.
 */
SocketRef Accept(const SocketRef & socket, ip_address * optRetLocalInfo = NULL);

/** Reads as many bytes as possible from the given socket and places them into (buffer).
 *  @param socket The socket to read from.
 *  @param buffer Buffer to write the bytes into.
 *  @param bufferSizeBytes Number of bytes in the buffer.
 *  @param socketIsBlockingIO Pass in true if the given socket is set to use blocking I/O, or false otherwise.
 *  @return The number of bytes read into (buffer), or a negative value if there was an error.
 *          Note that this value may be smaller than (bufferSizeBytes).
 */
int32 ReceiveData(const SocketRef & socket, void * buffer, uint32 bufferSizeBytes, bool socketIsBlockingIO);

/** Identical to ReceiveData(), except that this function's logic is adjusted to handle UDP semantics properly. 
 *  @param socket The socket to read from.
 *  @param buffer Buffer to write the bytes into.
 *  @param bufferSizeBytes Number of bytes in the buffer.
 *  @param socketIsBlockingIO Pass in true if the given socket is set to use blocking I/O, or false otherwise.
 *  @param optRetFromIP If set to non-NULL, then on success the ip_address this parameter points to will be filled in
 *                      with the IP address that the received data came from.  Defaults to NULL.
 *  @param optRetFromPort If set to non-NULL, then on success the uint16 this parameter points to will be filled in
 *                      with the source port that the received data came from.  Defaults to NULL.
 *  @return The number of bytes read into (buffer), or a negative value if there was an error.
 *          Note that this value may be smaller than (bufferSizeBytes).
 */
int32 ReceiveDataUDP(const SocketRef & socket, void * buffer, uint32 bufferSizeBytes, bool socketIsBlockingIO, ip_address * optRetFromIP = NULL, uint16 * optRetFromPort = NULL);

/** Transmits as many bytes as possible from the given buffer over the given socket.
 *  @param socket The socket to transmit over.
 *  @param buffer Buffer to read the outgoing bytes from.
 *  @param bufferSizeBytes Number of bytes to send.
 *  @param socketIsBlockingIO Pass in true if the given socket is set to use blocking I/O, or false otherwise.
 *  @return The number of bytes sent from (buffer), or a negative value if there was an error.
 *          Note that this value may be smaller than (bufferSizeBytes).
 */
int32 SendData(const SocketRef & socket, const void * buffer, uint32 bufferSizeBytes, bool socketIsBlockingIO);

/** Similar to SendData(), except that this function's logic is adjusted to handle UDP semantics properly.
 *  @param socket The socket to transmit over.
 *  @param buffer Buffer to read the outgoing bytes from.
 *  @param bufferSizeBytes Number of bytes to send.
 *  @param socketIsBlockingIO Pass in true if the given socket is set to use blocking I/O, or false otherwise.
 *  @param optDestIP If set to non-zero, the data will be sent to the given IP address.  Otherwise it will
 *                   be sent to the socket's current IP address (see SetUDPSocketTarget()).  Defaults to zero.
 *  @param destPort If set to non-zero, the data will be sent to the specified port.  Otherwise it will
 *                  be sent to the socket's current destination port (see SetUDPSocketTarget()).  Defaults to zero.
 *  @return The number of bytes sent from (buffer), or a negative value if there was an error.
 *          Note that this value may be smaller than (bufferSizeBytes).
 */
int32 SendDataUDP(const SocketRef & socket, const void * buffer, uint32 bufferSizeBytes, bool socketIsBlockingIO, const ip_address & optDestIP = invalidIP, uint16 destPort = 0);

/** This function initiates a non-blocking connection to the given host IP address and port.
  * It will return the created socket, which may or may not be fully connected yet.
  * If it is connected, (retIsReady) will be to true, otherwise it will be set to false.
  * If (retIsReady) is false, then you can use select() to find out when the state of the
  * socket has changed:  select() will return ready-to-write on the socket when it is 
  * fully connected (or when the connection fails), at which point you can call 
  * FinalizeAsyncConnect() on the socket:  if FinalizeAsyncConnect() succeeds, the connection 
  * succeeded; if not, the connection failed.
  * @param hostIP 32-bit IP address to connect to (hostname isn't used as hostname lookups can't be made asynchronous AFAIK)
  * @param port Port to connect to.
  * @param retIsReady On success, this bool is set to true iff the socket is ready to use, or 
  *                   false to indicate that an asynchronous connection is now in progress.
  * @return a non-negative sockfd which is in the process of connecting on success, or -1 on error.
  */
SocketRef ConnectAsync(const ip_address & hostIP, uint16 port, bool & retIsReady);

/** When a socket that was connecting asynchronously finally
  * selects ready-for-write to indicate that the asynchronous connect 
  * attempt has reached a conclusion, call this method.  It will finalize
  * the connection and make it ready for use.
  * @param socket The socket that was connecting asynchronously
  * @returns B_NO_ERROR if the connection is ready to use, or B_ERROR if the connect failed.
  * @note Under Windows, select() won't return ready-for-write if the connection fails... instead
  *       it will select-notify for your socket on the exceptions fd_set (if you provided one).
  *       Once this happens, there is no need to call FinalizeAsyncConnect() -- the fact that the
  *       socket notified on the exceptions fd_set is enough for you to know the asynchronous connection
  *       failed.  Successful asynchronous connect()s do exhibit the standard (select ready-for-write)
  *       behaviour, though.
  */
status_t FinalizeAsyncConnect(const SocketRef & socket);

/** Shuts the given socket down.  (Note that you don't generally need to call this function; it's generally
 *  only useful if you need to half-shutdown the socket, e.g. stop the output direction but not the input
 *  direction)
 *  @param socket The socket to shutdown communication on. 
 *  @param disableReception If true, further reception of data will be disabled on this socket.
 *  @param disableTransmission If true, further transmission of data will be disabled on this socket.
 *  @return B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t ShutdownSocket(const SocketRef & socket, bool disableReception = true, bool disableTransmission = true);

/** Convenience function for creating a TCP socket that is listening on a given local port for incoming TCP connections.
 *  @param port Which port to listen on, or 0 to have the system should choose a port for you
 *  @param maxbacklog Maximum connection backlog to allow for (defaults to 20)
 *  @param optRetPort If non-NULL, the uint16 this value points to will be set to the actual port bound to (useful when you want the system to choose a port for you)
 *  @param optInterfaceIP Optional IP address of the local network interface to listen on.  If left unspecified, or
 *                        if passed in as (invalidIP), then this socket will listen on all available network interfaces.
 *  @return the non-negative sockfd if the port was bound successfully, or -1 if the attempt failed.
 */
SocketRef CreateAcceptingSocket(uint16 port, int maxbacklog = 20, uint16 * optRetPort = NULL, const ip_address & optInterface = invalidIP);

/** Translates the given 4-byte IP address into a string representation.
 *  @param address The 4-byte IP address to translate into text.
 *  @param outBuf Buffer where the NUL-terminated ASCII representation of the string will be placed.
 *                This buffer must be at least 16 bytes long if compiled in IPv4 mode,
 *                or at least 46 bytes long if -DMUSCLE_USE_IPV6 is enabled in the Makefile.
 */
void Inet_NtoA(const ip_address & address, char * outBuf);

/** A more convenient version of Inet_Ntoa().  Given an IP address, returns a 
  * human-readable String representation of that address (e.g. "192.168.0.1",
  * or IPv6 equivalent if -DMUSCLE_USE_IPV6 is defined).
  */
String Inet_NtoA(const ip_address & ipAddress);

/** Returns true iff (s) is a well-formed IP address (e.g. "192.168.0.1")
  * @param (s) An ASCII string to check the formatting of
  * @returns true iff there are exactly four dot-separated integers between 0 and 255
  *               and no extraneous characters in the string.
  */
bool IsIPAddress(const char * s);

/** Given a dotted-quad IP address in ASCII format (e.g. "192.168.0.1"), returns
  * the equivalent IP address in ip_address (packed binary) form. 
  * @param buf numeric IP address in ASCII.
  * @returns IP address as a ip_address, or 0 on failure.
  */
ip_address Inet_AtoN(const char * buf);

/** Reurns the IP address that the given socket is connected to.
 *  @param socket The socket descriptor to find out info about.
 *  @param expandLocalhost If true, then if the peer's ip address is reported as 127.0.0.1, this 
 *                         function will attempt to determine the host machine's actual primary IP
 *                         address and return that instead.  Otherwise, 127.0.0.1 will be
 *                         returned in this case.
 *  @return The IP address on success, or 0 on failure (such as if the socket isn't valid and connected).
 */
ip_address GetPeerIPAddress(const SocketRef & socket, bool expandLocalhost);

/** Creates a pair of sockets that are connected to each other,
 *  so that any bytes you pass into one socket come out the other socket.
 *  This is useful when you want to wake up a thread that is blocked in select()...
 *  you have it select() on one socket, and you send a byte on the other.
 *  @param retSocket1 On success, this value will be set to the socket ID of the first socket.  Set to -1 on failure.
 *  @param retSocket2 On success, this value will be set to the socket ID of the second socket.  Set to -1 on failure.
 *  @param blocking Whether the two sockets should use blocking I/O or not.  Defaults to false.
 *  @return B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t CreateConnectedSocketPair(SocketRef & retSocket1, SocketRef & retSocket2, bool blocking = false);

/** Enables or disables blocking I/O on the given socket. 
 *  (Default for a socket is blocking mode enabled)
 *  @param socket the socket descriptor to act on.
 *  @param enabled Whether I/O on this socket should be enabled or not.
 *  @return B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t SetSocketBlockingEnabled(const SocketRef & socket, bool enabled);

/**
  * Turns Nagle's algorithm (output packet buffering/coalescing) on or off.
  * @param socket the socket descriptor to act on.
  * @param enabled If true, data will be held momentarily before sending, 
  *                to allow for bigger packets.  If false, each Write() 
  *                call will cause a new packet to be sent immediately.
  * @return B_NO_ERROR on success, B_ERROR on error.
  */
status_t SetSocketNaglesAlgorithmEnabled(const SocketRef & socket, bool enabled);

/**
  * Sets the size of the given socket's TCP send buffer to the specified
  * size (or as close to that size as is possible).
  * @param socket the socket descriptor to act on.
  * @param sendBufferSizeBytes New size of the TCP send buffer, in bytes.
  * @returns B_NO_ERROR on success, or B_ERROR on failure.
  */
status_t SetSocketSendBufferSize(const SocketRef & socket, uint32 sendBufferSizeBytes);

/**
  * Sets the size of the given socket's TCP receive buffer to the specified
  * size (or as close to that size as is possible).
  * @param socket the socket descriptor to act on.
  * @param receiveBufferSizeBytes New size of the TCP receive buffer, in bytes.
  * @returns B_NO_ERROR on success, or B_ERROR on failure.
  */
status_t SetSocketReceiveBufferSize(const SocketRef & socket, uint32 receiveBufferSizeBytes);

/** Convenience function:  Won't return for a given number of microsends.
 *  @param micros The number of microseconds to wait for.
 *  @return B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t Snooze64(uint64 microseconds);

/** Set a user-specified IP address to return from GetHostByName() and GetPeerIPAddress() instead of 127.0.0.1.
  * Note that this function <b>does not</b> change the computer's IP address -- it merely changes what
  * the aforementioned functions will report.
  * @param ip New IP address to return instead of 127.0.0.1, or 0 to disable this override.
  */
void SetLocalHostIPOverride(const ip_address & ip);

/** Returns the user-specified IP address that was previously set by SetLocalHostIPOverride(), or 0
  * if none was set.  Note that this function <b>does not</b> report the local computer's IP address,
  * unless you previously called SetLocalHostIPOverride() with that address.
  */
ip_address GetLocalHostIPOverride();

/** Creates and returns a socket that can be used for UDP communications.
 *  Returns a negative value on error, or a non-negative socket handle on
 *  success.  You'll probably want to call BindUDPSocket() or SetUDPSocketTarget()
 *  after calling this method.
 */
SocketRef CreateUDPSocket();

/** Attempts to given UDP socket to the given port.  
 *  @param sock The UDP socket (previously created by CreateUDPSocket())
 *  @param port UDP port ID to bind the socket to.  If zero, the system will choose a port ID.
 *  @param optRetPort if non-NULL, then on successful return the value this pointer points to will contain
 *                    the port ID that the socket was bound to.  Defaults to NULL.
 *  @param optFrom If non-zero, then the socket will be bound in such a way that only data
 *                 packets addressed to this IP address will be accepted.  Defaults to zero,
 *                 meaning that packets addressed to any of this machine's IP addresses will
 *                 be accepted.  (This parameter is typically only useful on machines with
 *                 multiple IP addresses) 
 *  @param allowShared If set to true, the port will be set up so that multiple processes
 *                     can bind to it simultaneously.  This is useful for sockets that are 
 *                     to be receiving broadcast UDP packets, since then you can run multiple
 *                     UDP broadcast receivers on a single computer. 
 *  @returns B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t BindUDPSocket(const SocketRef & sock, uint16 port, uint16 * optRetPort = NULL, const ip_address & optFrom = invalidIP, bool allowShared = false);

/** Set the target/destination address for a UDP socket.  After successful return
 *  of this function, any data that is written to the UDP socket will be sent to this
 *  IP address and port.  This function is guaranteed to return quickly.
 *  @param sock The UDP socket to send to (previously created by CreateUDPSocket()).
 *  @param remoteIP Remote IP address that data should be sent to.
 *  @param remotePort Remote UDP port ID that data should be sent to.
 *  @returns B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t SetUDPSocketTarget(const SocketRef & sock, const ip_address & remoteIP, uint16 remotePort);

/** Enable/disable sending of broadcast packets on the given UDP socket.
 *  @param sock UDP socket to enable or disable the sending of broadcast UDP packets with.
 *              (Note that the default state of newly created UDP sockets is broadcast-disabled)
 *  @param broadcast True if broadcasting should be enabled, false if broadcasting should be disabled.
 *  @returns B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t SetUDPSocketBroadcastEnabled(const SocketRef & sock, bool broadcast);

/** As above, except that the remote host is specified by hostname instead of IP address.
 *  Note that this function may take involve a DNS lookup, and so may take a significant
 *  amount of time to complete.
 *  @param sock The UDP socket to send to (previously created by CreateUDPSocket()).
 *  @param remoteHostName Name of remote host (e.g. "www.mycomputer.com" or "132.239.50.8")
 *  @param remotePort Remote UDP port ID that data should be sent to.
  * @param expandLocalhost If true, then if (name) corresponds to 127.0.0.1, this function
  *                        will attempt to determine the host machine's actual primary IP
  *                        address and return that instead.  Otherwise, 127.0.0.1 will be
  *                        returned in this case.  Defaults to false.
 *  @returns B_NO_ERROR on success, or B_ERROR on failure.
 */
status_t SetUDPSocketTarget(const SocketRef & sock, const char * remoteHostName, uint16 remotePort, bool expandLocalhost = false);

/** This little container class is used to return data from the GetNetworkInterfaceInfos() function, below */
class NetworkInterfaceInfo
{
public:
   NetworkInterfaceInfo();
   NetworkInterfaceInfo(const char * name, const char * desc, const ip_address & ip, const ip_address & netmask, const ip_address & broadcastIP);

   /** Returns the name of this interface, or "" if the name is not known. */
   const String & GetName() const {return _name;}

   /** Returns a (human-readable) description of this interface, or "" if a description is unavailable. */
   const String & GetDescription() const {return _desc;}

   /** Returns the IP address of this interface */
   const ip_address & GetLocalAddress() const {return _ip;}

   /** Returns the netmask of this interface */
   const ip_address & GetNetmask() const {return _netmask;}

   /** If this interface is a point-to-point interface, this method returns the IP
     * address of the machine at the remote end of the interface.  Otherwise, this
     * method returns the broadcast address for this interface.
     */
   const ip_address & GetBroadcastAddress() const {return _broadcastIP;}

private:
   String _name;
   String _desc;
   ip_address _ip;
   ip_address _netmask;
   ip_address _broadcastIP;
};

/** This function queries the local OS for information about all available network
  * interfaces.  Note that this method is only implemented for some OS's (Linux,
  * MacOS/X, Windows), and that on other OS's it may just always return B_ERROR.
  * @param results On success, zero or more NetworkInterfaceInfo objects will
  *                be added to this Queue for you to look at.
  * @returns B_NO_ERROR on success, or B_ERROR on failure (out of memory,
  *          call not implemented for the current OS, etc)
  */
status_t GetNetworkInterfaceInfos(Queue<NetworkInterfaceInfo> & results);

/** This simple class holds an IP address and a port number, and lets you do
  * useful things on the two such as using them as key values in a hash table,
  * converting them to/from user-readable strings, etc.
  */
class IPAddressAndPort
{
public:
   /** Default constructor.   Creates an IPAddressAndPort object with the address field
     * set to (invalidIP) and the port field set to zero.
     */
   IPAddressAndPort() : _ip(invalidIP), _port(0) {/* empty */}

   /** Explicit constructor
     * @param ip The IP address to represent
     * @param port The port number to represent
     */
   IPAddressAndPort(const ip_address & ip, uint16 port) : _ip(ip), _port(port) {/* empty */}

   /** Convenience constructor.  Calling this is equivalent to creating an IPAddressAndPort
     * object and then calling SetFromString() on it with the given arguments.
     */
   IPAddressAndPort(const String & s, uint16 defaultPort, bool allowDNSLookups) {SetFromString(s(), defaultPort, allowDNSLookups);}

   /** Copy constructor */
   IPAddressAndPort(const IPAddressAndPort & rhs) : _ip(rhs._ip), _port(rhs._port) {/* empty */}

   bool operator == (const IPAddressAndPort & rhs) const {return (_ip == rhs._ip)&&(_port == rhs._port);}
   bool operator != (const IPAddressAndPort & rhs) const {return !(*this==rhs);}

   bool operator < (const IPAddressAndPort & rhs) const {return ((_ip < rhs._ip)||((_ip == rhs._ip)&&(_port < rhs._port)));}
   bool operator > (const IPAddressAndPort & rhs) const {return ((_ip > rhs._ip)||((_ip == rhs._ip)&&(_port > rhs._port)));}
   bool operator <= (const IPAddressAndPort & rhs) const {return !(*this>rhs);}
   bool operator >= (const IPAddressAndPort & rhs) const {return !(*this<rhs);}

   /** HashCode returns a usable 32-bit hash code value for this object, based on its contents. */
#ifdef MUSCLE_USE_IPV6
   uint32 HashCode() const {return _ip.HashCode()+_port;}
#else
   uint32 HashCode() const {return _ip+_port;}
#endif

   /** Returns this object's current IP address */
   const ip_address & GetIPAddress() const {return _ip;}

   /** Returns this object's current port number */
   uint16 GetPort() const {return _port;}

   /** Sets this object's IP address to (ip) */
   void SetIPAddress(const ip_address & ip) {_ip = ip;}

   /** Sets this object's port number to (port) */
   void SetPort(uint16 port) {_port = port;}

   /** Resets this object to its default state; as if it had just been created by the default constructor. */
   void Reset() {_ip = invalidIP; _port = 0;}

   /** Returns true iff both our IP address and port number are valid (i.e. non-zero) */
   bool IsValid() const {return ((_ip != invalidIP)&&(_port != 0));}

   /** Sets this object's state from the passed-in character string.
     * IPv4 address may be of the form "192.168.1.102", or of the form "192.168.1.102:2960".
     * When -DMUSCLE_USE_IPV6 is enabled, IPv6 address (e.g. "::1") are also supported.
     * Note that if you want to specify an IPv6 address and a port number at the same
     * time, you will need to enclose the IPv6 address in brackets, like this:  "[::1]:2960"
     * @param s The user-readable IP-address string, with optional port specification
     * @param defaultPort What port number to assign if the string does not specify a port number.
     * @param allowDNSLookups If true, this function will try to interpret non-numeric host names
     *                        e.g. "www.google.com" by doing a DNS lookup.  If false, then no
     *                        name resolution will be attempted, and only numeric IP addesses will be parsed.
     */
   void SetFromString(const String & s, uint16 defaultPort, bool allowDNSLookups);

   /** Returns a string representation of this object, similar to the forms
     * described in the SetFromString() documentation, above.
     * @param includePort If true, the port will be included in the string.  Defaults to true.
     */
   String ToString(bool includePort = true) const;

private:
   ip_address _ip;
   uint16 _port;
};

template <>
class HashFunctor<IPAddressAndPort>
{
public:
   uint32 operator () (const IPAddressAndPort & x) const {return x.HashCode();}
};


END_NAMESPACE(muscle);

#endif

