/* This file is Copyright 2003 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleMessageIOGateway_h
#define MuscleMessageIOGateway_h

#include "iogateway/AbstractMessageIOGateway.h"
#include "util/ByteBuffer.h"

#ifdef MUSCLE_ENABLE_ZLIB_ENCODING
# include "zlib/ZLibCodec.h"
#endif

BEGIN_NAMESPACE(muscle);

/**
 * Encoding IDs.  As of MUSCLE 2.40, we support vanilla MUSCLE_MESSAGE_ENCODING_DEFAULT and 9 levels of zlib compression!
 */
enum {
   MUSCLE_MESSAGE_ENCODING_DEFAULT = 1164862256, // 'Enc0',  /**< just plain ol' flattened Message objects, with no special encoding */
#ifdef MUSCLE_ENABLE_ZLIB_ENCODING
   MUSCLE_MESSAGE_ENCODING_ZLIB_1,                           /**< lowest level of zlib compression (most CPU-efficient) */
   MUSCLE_MESSAGE_ENCODING_ZLIB_2,
   MUSCLE_MESSAGE_ENCODING_ZLIB_3,
   MUSCLE_MESSAGE_ENCODING_ZLIB_4,
   MUSCLE_MESSAGE_ENCODING_ZLIB_5,
   MUSCLE_MESSAGE_ENCODING_ZLIB_6,                           /**< This is the recommended CPU vs space-savings setting for zlib */
   MUSCLE_MESSAGE_ENCODING_ZLIB_7,
   MUSCLE_MESSAGE_ENCODING_ZLIB_8,
   MUSCLE_MESSAGE_ENCODING_ZLIB_9,                           /**< highest level of zlib compression (most space-efficient) */
#endif
   MUSCLE_MESSAGE_ENCODING_END_MARKER = MUSCLE_MESSAGE_ENCODING_DEFAULT+10  /**< Not a valid -- just here to mark the end of the range */
};

/** Callback function type for flatten/unflatten notification callbacks */
typedef void (*MessageFlattenedCallback)(MessageRef msgRef, void * userData);

/**
 * A "gateway" object that knows how to send/receive Messages over a wire, via a provided DataIO object. 
 * May be subclassed to change the byte-level protocol, or used as-is if the default protocol is desired.
 * If ZLib compression is desired, be sure to compile with -DMUSCLE_ENABLE_ZLIB_ENCODING
 *
 * The default protocol format used by this class is:
 *   -# 4 bytes (uint32) indicating the flattened size of the message
 *   -# 4 bytes (uint32) indicating the encoding type (should always be MUSCLE_MESSAGE_ENCODING_DEFAULT for now)
 *   -# n bytes of flattened Message (where n is the value specified in 1)
 *   -# goto 1 ...
 *
 * An example flattened Message byte structure is provided at the bottom of the
 * MessageIOGateway.h header file.
 */
class MessageIOGateway : public AbstractMessageIOGateway
{
public:
   /** 
    *  Constructor.
    *  @param outgoingEncoding The byte-stream format the message should be encoded into.
    *                          Should be one of the MUSCLE_MESSAGE_ENCODING_* values.
    *                          Default is MUSCLE_MESSAGE_ENCODING_DEFAULT, meaning that no
    *                          compression will be done.  Note that to use any of the
    *                          MUSCLE_MESSAGE_ENCODING_ZLIB_* encodings, you MUST have
    *                          defined the compiler symbol -DMUSCLE_ENABLE_ZLIB_ENCODING.
    */
   MessageIOGateway(int32 outgoingEncoding = MUSCLE_MESSAGE_ENCODING_DEFAULT);

   /**
    *  Destructor.
    *  Deletes the held DataIO object.
    */
   virtual ~MessageIOGateway();

   virtual bool HasBytesToOutput() const;
   virtual void Reset();

   /**
    * Lets you specify a function that will be called every time an outgoing
    * Message is about to be flattened by this gateway.  You may alter the
    * Message at this time, if you need to.
    * @param cb Callback function to call.
    * @param ud User data; set this to any value you like.
    */
   void SetAboutToFlattenMessageCallback(MessageFlattenedCallback cb, void * ud) {_aboutToFlattenCallback = cb; _aboutToFlattenCallbackData = ud;}

   /**
    * Lets you specify a function that will be called every time an outgoing
    * Message has been flattened by this gateway.
    * @param cb Callback function to call.
    * @param ud User data; set this to any value you like.
    */
   void SetMessageFlattenedCallback(MessageFlattenedCallback cb, void * ud) {_flattenedCallback = cb; _flattenedCallbackData = ud;}

   /**
    * Lets you specify a function that will be called every time an incoming
    * Message has been unflattened by this gateway.
    * @param cb Callback function to call.
    * @param ud User data; set this to any value you like.
    */
   void SetMessageUnflattenedCallback(MessageFlattenedCallback cb, void * ud) {_unflattenedCallback = cb; _unflattenedCallbackData = ud;}

   /**
    * Lets you specify the maximum allowable size for an incoming flattened Message.
    * Doing so lets you limit the amount of memory a remote computer can cause your
    * computer to attempt to allocate.  Default max message size is MUSCLE_NO_LIMIT 
    * (or about 4 gigabytes)
    * @param maxBytes New incoming message size limit, in bytes.
    */
   void SetMaxIncomingMessageSize(uint32 maxBytes) {_maxIncomingMessageSize = maxBytes;}

   /** Returns the current maximum incoming message size, as was set above. */
   uint32 GetMaxIncomingMessageSize() const {return _maxIncomingMessageSize;}

   /** Returns our encoding method, as specified in the constructor or via SetOutgoingEncoding(). */
   int32 GetOutgoingEncoding() const {return _outgoingEncoding;}

   /** Call this to change the encoding this gateway applies to outgoing Messages.
     * Note that the encoding change will take place starting with the next Message
     * that is actually sent, so if any Messages are currently Queued up to be sent,
     * they will be sent using the new encoding.
     * Note that to use any of the MUSCLE_MESSAGE_ENCODING_ZLIB_* encodings, 
     * you MUST have defined the compiler symbol -DMUSCLE_ENABLE_ZLIB_ENCODING.
     * @param ec Encoding type to use.  Should be one of the MUSCLE_MESSAGE_ENCODING_* constants.
     */
   void SetOutgoingEncoding(int32 ec) {_outgoingEncoding = ec;}

protected:
   virtual int32 DoOutputImplementation(uint32 maxBytes = MUSCLE_NO_LIMIT);
   virtual int32 DoInputImplementation(AbstractGatewayMessageReceiver & receiver, uint32 maxBytes = MUSCLE_NO_LIMIT);

   /**
    * Flattens a specified Message object into a flat ByteBuffer object.
    * @param msgRef Reference to a Message to flatten into a byte array.
    * @param header Pointer to a buffer that is (GetHeaderSize()) bytes long.
    *               The appropriate header bytes should be written in to this buffer.
    * @return A reference to a ByteBuffer object containing the flattened Message data on success,
    *         or a NULL reference on failure.
    * The default implementation uses msg.Flatten() and then (optionally) ZLib compression to produce the bytes.
    */
   virtual ByteBufferRef FlattenMessage(MessageRef msgRef, uint8 * header) const;

   /**
    * Unflattens a specified ByteBuffer object back into a MessageRef object.
    * @param bufRef Reference to a ByteBuffer object to unflatten back into a Message.
    * @param header Points to the header bytes that were received for this Message.  This
    *               array is (GetHeaderSize()) bytes long.
    * @returns a Reference to a Message object containing the Message that was encoded in
    *          the ByteBuffer on success, or a NULL reference on failure.
    * The default implementation uses (optional) ZLib decompression (depending on the header bytes)
    * and then msg.Unflatten() to produce the Message.
    */
   virtual MessageRef UnflattenMessage(ByteBufferRef bufRef, const uint8 * header) const;
 
   /**
    * Returns the size of the pre-flattened-message header section, in bytes.
    * The default Message protocol uses an 8-byte header (4 bytes for encoding ID, 4 bytes for message size),
    * so the default implementation of this method always returns 8.
    */
   virtual uint32 GetHeaderSize() const;

   /**
    * Must Extract and returns the buffer body size from the given header.
    * Note that the returned size should NOT count the header bytes themselves!
    * @param header Points to the header of the message.  The header is GetHeaderSize() bytes long.
    * @return The number of bytes in the body of the message associated with (header), on success,
    *         or a negative value to indicate an error (invalid header, etc).
    */
   virtual int32 GetBodySize(const uint8 * header) const;

private:
#ifdef MUSCLE_ENABLE_ZLIB_ENCODING
   ZLibCodec * GetCodec(int32 newEncoding, ZLibCodec * & setCodec) const;
#endif

   class TransferBuffer
   {
   public:
      TransferBuffer() : _offset(0) {/* empty */}

      void Reset()
      {
         _buffer.Reset();
         _offset = 0;
      }

      ByteBufferRef _buffer;
      uint32 _offset;
   };

   status_t SendData(TransferBuffer & buf, int32 & sentBytes, uint32 & maxBytes);
   status_t ReadData(TransferBuffer & buf, int32 & readBytes, uint32 & maxBytes);

   TransferBuffer _sendHeaderBuffer;
   TransferBuffer _sendBodyBuffer;

   TransferBuffer _recvHeaderBuffer;
   TransferBuffer _recvBodyBuffer;

   uint32 _maxIncomingMessageSize;
   int32 _outgoingEncoding;
  
   MessageFlattenedCallback _aboutToFlattenCallback;
   void * _aboutToFlattenCallbackData;

   MessageFlattenedCallback _flattenedCallback;
   void * _flattenedCallbackData;

   MessageFlattenedCallback _unflattenedCallback;
   void * _unflattenedCallbackData;

#ifdef MUSCLE_ENABLE_ZLIB_ENCODING
   mutable ZLibCodec * _sendCodec;
   mutable ZLibCodec * _recvCodec;
#endif
};

//////////////////////////////////////////////////////////////////////////////////
//
// Here is a commented example of a flattened Message's byte structure, using
// the MUSCLE_MESSAGE_ENCODING_DEFAULT encoding.
//
// When one uses a MessageIOGateway with the default encoding to send Messages, 
// it will send out series of bytes that looks like this.
//
// Note that this information is only helpful if you are trying to implement
// your own MessageIOGateway-compatible serialization/deserialization code.
// C++, Java, and Python programmers will have a much easier time if they use 
// the MessageIOGateway class provided in the MUSCLE archive, rather than 
// coding at the byte-stream level.
//
// The Message used in this example has a 'what' code value of 2 and the 
// following name/value pairs placed in it:
//
//  String field, name="!SnKy"   value="/*/*/beshare"
//  String field, name="session" value="123"
//  String field, name="text"    value="Hi!"
//
// Bytes in single quotes represent ASCII characters, bytes without quotes
// means literal decimal byte values.  (E.g. '2' means 50 decimal, 2 means 2 decimal)
//
// All occurences of '0' here indicate the ASCII digit zero (decimal 48), not the letter O.
//
// The bytes shown here should be sent across the TCP socket in 
// 'normal reading order': left to right, top to bottom.
//
// 88   0   0   0   (int32, indicates that total message body size is 88 bytes) (***)
// '0' 'c' 'n' 'E'  ('Enc0' == MUSCLE_MESSAGE_ENCODING_DEFAULT) (***)
//
// '0' '0' 'M' 'P'  ('PM00' == CURRENT_PROTOCOL_VERSION)
//  2   0   0   0   (2      == NET_CLIENT_NEW_CHAT_TEXT, the message's 'what' code)
//  3   0   0   0   (3      == Number of name/value pairs in this message)
//  6   0   0   0   (6      == Length of first name, "!SnKy", include NUL byte) 
// '!' 'S' 'n' 'K'  (Field name ASCII bytes.... "!SnKy")
// 'y'  0           (last field name ASCII byte and the NUL terminator byte) 
// 'R' 'T' 'S' 'C'  ('CSTR' == B_STRING_TYPE; i.e. this value is a string)
// 13   0   0   0   (13     == Length of value string including NUL byte)
// '/' '*' '/' '*'  (Field value ASCII bytes.... "/*/*/beshare")
// '/' 'b' 'e' 's'  (....)
// 'h' 'a' 'r' 'e'  (....)
//  0               (NUL terminator byte for the ASCII value)
//  8   0   0   0   (8      == Length of second name, "session", including NUL)
// 's' 'e' 's' 's'  (Field name ASCII Bytes.... "session")
// 'i' 'o' 'n'  0   (rest of field name ASCII bytes and NUL terminator)
// 'R' 'T' 'S' 'C'  ('CSTR' == B_STRING_TYPE; i.e. this value is a string)
//  4   0   0   0   (4      == Length of value string including NUL byte)
// '1' '2' '3'  0   (Field value ASCII bytes... "123" plus NUL byte)
//  5   0   0   0   (5      == Length of third name, "text", including NUL)
// 't' 'e' 'x' 't'  (Field name ASCII bytes... "text")
//  0               (NUL byte terminator for field name)
// 'R' 'T' 'S' 'C'  ('CSTR' == B_STRING_TYPE; i.e. this value is a string)
//  4   0   0   0   (3      == Length of value string including NUL byte)
// 'H' 'i' '!'  0   (Field value ASCII Bytes.... "Hi!" plus NUL byte)
//
// [that's the complete byte sequence; to transmit another message, 
//  you would start again at the top, with the next message's 
//  message-body-length-count]
//
// (***) The bytes in this field should not be included when tallying the message body size!
//
//////////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(muscle);

#endif
