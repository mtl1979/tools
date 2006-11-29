/* This file is Copyright 2007 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */

#ifndef MuscleDataIO_h
#define MuscleDataIO_h

#include "util/RefCount.h"

BEGIN_NAMESPACE(muscle);
 
/** Abstract base class for a byte-stream Data I/O interface, similar to Be's BDataIO.  */
class DataIO : public RefCountable
{
public:
   /** Values to pass in to DataIO::Seek()'s second parameter */
   enum {
      IO_SEEK_SET = 0,
      IO_SEEK_CUR,
      IO_SEEK_END,
      NUM_IO_SEEKS
   };

   /** Default Constructor */
   DataIO() {/* empty */}

   /** Virtual destructor, to keep C++ honest.  */
   virtual ~DataIO() {/* empty */}

   /** Tries to place (size) bytes of new data into (buffer).  Returns the
    *  actual number of bytes placed, or a negative value if there
    *  was an error.
    *  @param buffer Buffer to write the bytes into
    *  @param size Number of bytes in the buffer.
    *  @return Number of bytes read, or -1 on error.   
    */
   virtual int32 Read(void * buffer, uint32 size) = 0;

   /** Takes (size) bytes from (buffer) and pushes them in to the
    *  outgoing I/O stream.  Returns the actual number of bytes 
    *  read from (buffer) and pushed, or a negative value if there
    *  was an error.
    *  @param buffer Buffer to read the bytes from.
    *  @param size Number of bytes in the buffer.
    *  @return Number of bytes written, or -1 on error.        
    */
   virtual int32 Write(const void * buffer, uint32 size) = 0;

   /**
    * Seek to a given position in the I/O stream.  
    * May not be supported by a DataIO subclass, in 
    * which case B_ERROR will always be returned.
    * @param offset Byte offset to seek to or by (depending on the next arg)
    * @param whence Set this to IO_SEEK_SET if you want the offset to
    *               be relative to the start of the stream; or to 
    *               IO_SEEK_CUR if it should be relative to the current
    *               stream position, or IO_SEEK_END if it should be
    *               backwards from the end of the stream.
    * @return B_NO_ERROR on success, or B_ERROR on failure or if unsupported.
    */
   virtual status_t Seek(int64 offset, int whence) = 0;

   /**
    * Should return the current position, in bytes, of the stream from 
    * its start position, or -1 if the current position is not known.
    */
   virtual int64 GetPosition() const = 0;

   /** 
    * Returns the max number of microseconds to allow
    * for an output stall, before presuming that the I/O is hosed.
    * Default implementation returns MUSCLE_TIME_NEVER, aka no limit.
    */
   virtual uint64 GetOutputStallLimit() const {return MUSCLE_TIME_NEVER;}

   /**
    * Flushes the output buffer, if possible.  For some implementations,
    * this is a no-op.  For others (e.g. TCPSocketDataIO) this can be
    * called to improve latency of outgoing data blocks.
    */
   virtual void FlushOutput() = 0;

   /** 
    * Closes the connection.  After calling this method, the
    * DataIO object should not be used any more.
    */
   virtual void Shutdown() = 0;

   /**
    * If this DataIO is usable with select(), this method should return
    * the socket FD to select on to watch this DataIO.  If it isn't usable
    * with select, then this method should return -1.
    *
    * Note that the only thing you are allowed to do with the returned file descriptor
    * is pass it to select().  For all other operations, use the appropriate
    * methods in the DataIO interface.  If you attempt to do any other I/O operations
    * on this file descriptor directly, the results are non-portable and undefined 
    * and will probably break your program.
    */
   virtual int GetSelectSocket() const = 0;

   /**
    * Optional interface for returning information on when a given byte
    * returned by the previous Read() call was received.  Not implemented 
    * by default, and not implemented by any of the standard MUSCLE DataIO 
    * subclasses. (Used by an LCS dataIO class that needs precision timing)
    * @param whichByte Index of the byte in the previously returned 
    *                  read-buffer that you are interested in.
    * @param retStamp On success, this value is set to the timestamp
    *                 of the byte.
    * @return B_NO_ERROR if a timestamp was written into (retStamp),
    *                    otherwise B_ERROR.  Default implementation
    *                    always returns B_ERROR.
    */
   virtual status_t GetReadByteTimeStamp(int32 /*whichByte*/, uint64 & /*retStamp*/) const {return B_ERROR;}

   /**
    * Optional:  If your DataIO subclass is holding buffered data that it wants
    *            to output as soon as possible but hasn't been able to yet,
    *            then override this method to return true, and that will cause
    *            FlushBufferedOutput() to be called ASAP.  Default implementation
    *            always returns false.
    */
   virtual bool HasBufferedOutput() const {return false;}

   /**
    * Optional:  If this DataIO is holding any buffered output data, this method should 
    *            be implemented to Write() as much of that data as possible.  Default 
    *            implementation is a no-op.
    */
   virtual void WriteBufferedOutput() {/* empty */}

   /** Convenience method:  Calls Write() in a loop until the entire buffer is written, or
     * until an error occurs.  This method should only be used in conjunction with 
     * blocking I/O; it will not work reliably with non-blocking I/O.
     * @param buffer Pointer to the first byte of the buffer to write.
     * @param size Number of bytes to write
     * @return The number of bytes that were actually written.  On success,
     *         This will be equal to (size).  On failure, it will be a smaller value.
     */
   uint32 WriteFully(const void * buffer, uint32 size);
   
   /** Convenience method:  Calls Read() in a loop until the entire buffer is written, or
     * until an error occurs.  This method should only be used in conjunction with 
     * blocking I/O; it will not work reliably with non-blocking I/O.
     * @param buffer Pointer to the first byte of the buffer to read.
     * @param size Number of bytes to read
     * @return The number of bytes that were actually read.  On success,
     *         This will be equal to (size).  On failure, it will be a smaller value.
     */
   uint32 ReadFully(void * buffer, uint32 size);

   /** Convenience method:  Determines the length of this DataIO stream by Seek()'ing
     * to the end of the stream, recording the current seek position, and then
     * Seek()'ing back to the previous position in the stream.  Of course this only
     * works with DataIOs that support seeking and have a fixed length.
     * @returns The total length of this DataIO in bytes, or -1 on error.
     */
   virtual int64 GetLength();
};

typedef Ref<DataIO> DataIORef;

END_NAMESPACE(muscle);

#endif
