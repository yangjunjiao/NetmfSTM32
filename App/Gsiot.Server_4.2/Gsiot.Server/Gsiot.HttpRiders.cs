/* Copyright (c) 2012 Oberon microsystems, Inc. (Switzerland)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

// Originally developed for the book
//   "Getting Started with the Internet of Things", by Cuno Pfister.
//   Copyright 2011 Cuno Pfister, Inc., 978-1-4493-9357-1.

// Version 1.0, for the .NET Micro Framework release 4.2.

// The namespace Gsiot.HttpRiders provides support for reading and writing
// HTTP messages, using the Socket API.
//
// For writing a message an HttpWriter instance is attached to a socket,
// the request/status line is written using the provided Write* methods,
// and then the message body (content) is written, if any. For writing
// the content, one or more calls to WriteContent are performed. At the
// end, the writer is detached and can later be reused again.
//
// For reading a message an HttpReader instance is attached to a socket,
// the request/status line is read using the provided Read* methods,
// and then the message body (content) is read, if any. For reading
// the content, one or more calls to ReadContent are performed. At the
// end, the reader is detached and can later be reused again.
//
// HttpReader and HttpWriter are designed to have a mostly static memory
// consumption, so that if there is sufficient memory to create them, it
// is most likely that they will not cause an out-of-memory condition
// (heap full) later on.

using System.IO;
using Gsiot.Contracts;

namespace Gsiot.HttpRiders
{
    /// <summary>
    /// Possible states of a reader or writer instance.
    /// BeforeContent
    ///     Currently writing/reading request line, status line, or header.
    /// InContent
    ///     Currently writing/reading the message content.
    /// TimeoutError
    ///     Nothing was sent/received until the Timeout has passed.
    /// ConnectionError
    ///     The connection was lost during writing/reading.
    /// SyntaxError
    ///     Unexpected read received as part of a request line, status
    ///     line, or header.
    /// </summary>
    public enum HttpStatus
    {
        BeforeContent, InContent,       // reader & writer "no error" codes
        ConnectionError,                // reader & writer error codes
        SyntaxError                     // reader-specific error code
    }

    /// <summary>
    /// Light-weight object for writing HTTP messages to sockets.
    /// An HttpWriter instance does not allocate any memory after
    /// it has been created, unless an exception occurs.
    /// </summary>
    public sealed class HttpWriter
    {
        Stream connection;  // not null between Attach and Detach

        // Data to be written is buffered first. When the buffer is full,
        // it is flushed and cleared.
        const int bufferLength = 100;
        byte[] buffer = new byte[bufferLength]; // where (buffer != null)
        int bufferPos;                          // where (bufferPos >= 0)
        // where (bufferPos <= bufferLength)

        /// <summary>
        /// Optional property with the Send timeout in milliseconds.
        /// Default: 5000 milliseconds (5 seconds)
        /// If the value -1 is set, no timeout occurs (wait forever).
        /// The value 0 indicates the default.
        /// If a value between 1 and 499 is set, the socket API will
        /// use 500 milliseconds as its minimum timeout value.
        /// </summary>
        public int Timeout { get; set; }

        /// <summary>
        /// Current status of the writer.
        /// </summary>
        public HttpStatus Status { get; internal set; }

        /// <summary>
        /// Attaches a writer to a socket.
        /// If the writer was previously used and an exception occurred,
        /// it can be used again anyway, because it will be completely re-
        /// initialized.
        /// Preconditions
        ///     connection != null
        ///     Timeout >= -1
        /// Postconditions
        ///     "writer is attached"
        /// </summary>
        /// <param name="connection">Open socket connection</param>
        public void Attach(Stream connection)
        {
            // Attach even works after an exception, even if the object
            // state is inconsistent.
            Contract.Requires(connection != null);
            Contract.Requires(Timeout >= -1);
            this.connection = connection;
            if (Timeout == 0) { Timeout = 5000; }    // 5 seconds default
            if (connection.CanTimeout)
            {
                connection.WriteTimeout = Timeout;
            }
            Status = HttpStatus.BeforeContent;
            bufferPos = 0;
        }

        void Flush()
        {
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                connection.Write(buffer, 0, bufferPos);
            }
            bufferPos = 0;
        }

        /// <summary>
        /// Writes a single ASCII character to the socket, as part of a
        /// HTTP request line, HTTP status line, or HTTP header.
        /// It *cannot* be used to write content, i.e., the HTTP message
        /// body. Use WriteContent for that purpose.
        /// If the writer is in an error state, this method does nothing.
        /// Preconditions
        ///     c >= (char)0
        ///     c lessEqual (char)127
        ///     "writer is attached"
        ///     (Status == HttpWriteStatus.BeforeContent) || "error"
        /// </summary>
        /// <param name="c">ASCII character to be written</param>
        public void WriteChar(char c)
        {
            Contract.Requires(c >= (char)0);
            Contract.Requires(c <= (char)127);

            var b = (int)c;
            Contract.Requires((b >= 0) && (b <= 127));
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                // bufferPos >= 0
                // bufferPos <= bufferLength
                if (bufferPos == bufferLength)
                {
                    Flush();
                    // bufferPos == 0
                }
                // bufferPos >= 0
                // bufferPos < bufferLength
                buffer[bufferPos] = (byte)b;
                bufferPos = bufferPos + 1;
                // bufferPos > 0
                // bufferPos <= bufferLength
            }
        }

        /// <summary>
        /// Writes an ASCII string to the socket, as part of a
        /// HTTP request line, HTTP status line, or HTTP header.
        /// It *cannot* be used to write content, i.e., the HTTP message
        /// body. Use WriteContent for that purpose.
        /// If the writer is in an error state, this method does nothing.
        /// Preconditions
        ///     s != null
        ///     "writer is attached"
        ///     (Status == HttpWriteStatus.BeforeContent) || "error"
        /// </summary>
        /// <param name="s">ASCII string to be written</param>
        public void WriteString(string s)
        {
            Contract.Requires(s != null);
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                var i = 0;
                int length = s.Length;
                while (i != length)
                {
                    WriteChar(s[i]);
                    i = i + 1;
                }
            }
        }

        /// <summary>
        /// Writes an ASCII string to the socket, as part of a
        /// HTTP request line, HTTP status line, or HTTP header.
        /// After the string, a carriage return and a newline character
        /// are written to the socket as well.
        /// It *cannot* be used to write content, i.e., the HTTP message
        /// body. Use WriteContent for that purpose.
        /// If the writer is in an error state, this method does nothing.
        /// Preconditions
        ///     s != null
        ///     "writer is attached"
        ///     (Status == HttpWriteStatus.BeforeContent) || "error"
        /// </summary>
        /// <param name="s">ASCII string to be written</param>
        public void WriteLine(string s)
        {
            Contract.Requires(s != null);
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                var i = 0;
                int length = s.Length;
                while (i != length)
                {
                    WriteChar(s[i]);
                    i = i + 1;
                }
                WriteChar('\r');
                WriteChar('\n');
            }
        }

        /// <summary>
        /// Writes a carriage return and a newline character to the
        /// socket. The two most recently written bytes must also
        /// have been a carriage return and a newline character.
        /// In the HTTP protocol, the sequence CR NL CR NL indicates
        /// the transition to the message body, i.e., the bytes
        /// immediately following these four bytes - if any - form the
        /// message content.
        /// This method must be cause after the last header is written
        /// and before the content is written!
        /// Preconditions
        ///     "writer is attached"
        ///     Status == HttpWriteStatus.BeforeContent
        /// Postconditions
        ///     Status == HttpWriteStatus.InContent
        /// </summary>
        public void WriteBeginOfContent()
        {
            Contract.Requires(connection != null);
            Contract.Requires(Status == HttpStatus.BeforeContent);
            WriteChar('\r');
            WriteChar('\n');
            Flush();
            if (Status == HttpStatus.BeforeContent)
            {
                Status = HttpStatus.InContent;
            }
        }

        /// <summary>
        /// Writes a number of bytes to the socket, as part of the message
        /// content. It cannot be used for writing request lines, status
        /// lines, or headers.
        /// If the writer is in an error state, this method does nothing.
        /// Preconditions
        ///     buffer != null
        ///     offset >= 0
        ///     count >= 0
        ///     offset + count lessEqual buffer.Length
        ///     "writer is attached"
        ///     (Status == HttpWriteStatus.InContent) || "error"
        /// </summary>
        /// <param name="buffer">Some bytes from this buffer will be
        /// written</param>
        /// <param name="offset">Index of first byte to be written</param>
        /// <param name="count">Number of bytes to be written</param>
        public void WriteContent(byte[] buffer, int offset, int count)
        {
            // This method may be called multiple times, to send
            // content incrementally.
            Contract.Requires(buffer != null);
            Contract.Requires(offset >= 0);
            Contract.Requires(count >= 0);
            Contract.Requires(offset + count <= buffer.Length);
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.InContent);
                connection.Write(buffer, offset, count);
            }
        }

        /// <summary>
        /// Detaches an attached writer from a socket.
        /// Preconditions
        ///     "writer is attached"
        /// Postconditions
        ///     "writer is not attached"
        /// </summary>
        public void Detach()
        {
            Contract.Requires(connection != null);
            connection = null;
            // Don't change the Status property, it may still be
            // needed by the caller for error analysis.
        }
    }

    // -------------------------------------------------------------------

    /// <summary>
    /// Light-weight object for reading HTTP messages from sockets.
    /// An HttpWriter instance does not allocate any memory after
    /// it has been created, unless an exception occurs.
    /// </summary>
    public sealed class HttpReader
    {
        Stream connection;  // not null between Attach and Detach

        // Data is received byte by byte, up to the message content which
        // is received in larger chunk(s).
        byte[] buffer = new byte[1];            // where (buffer != null)

        // Symbols are copied from byte buffer to a symbol buffer.
        // Received characters that don't fit into a symbol due to the
        // symbol buffer's size limit do not go through the symbol buffer,
        // i.e., they are truncated.
        const int symbolBufferCapacity = 100;
        char[] symbolBuffer = new char[symbolBufferCapacity];
        // where (symbolBuffer != null)
        int symbolLength;                   // where (symbolLength >= 0)
        // where (symbolLength <= symbolBufferCapacity)


        /// <summary>
        /// Optional property with the Receive timeout in milliseconds.
        /// Default: 30000 milliseconds (30 seconds)
        /// If the value -1 is set, no timeout occurs (wait forever).
        /// The value 0 indicates the default.
        /// </summary>
        public int Timeout { get; set; }

        /// <summary>
        /// Current status of the reader.
        /// </summary>
        public HttpStatus Status { get; internal set; }

        /// <summary>
        /// Attaches an unattached reader to a socket.
        /// If the reader was previously used and an exception occurred,
        /// it can be used again anyway, because it will be completely re-
        /// initialized.
        /// Preconditions
        ///     connection != null
        ///     Timeout >= -1
        /// Postconditions
        ///     "reader is attached"
        /// </summary>
        /// <param name="connection">Open socket connection</param>
        public void Attach(Stream connection)
        {
            Contract.Requires(connection != null);
            Contract.Requires(Timeout >= 0);
            if (Timeout == 0) { Timeout = 30000; }    // 30 seconds default
            this.connection = connection;
            if (connection.CanTimeout)
            {
                connection.ReadTimeout = Timeout;
            }
            symbolLength = 0;
            Status = HttpStatus.BeforeContent;
        }

        // Receive one character from socket connection.
        // Postconditions
        //     (Status != HttpStatus.BeforeContent) => (Result == (char)0)
        //     (symbolLength > symbolBufferCapacity) => "overflow"
        char ReadChar()
        {
            Contract.Requires(Status == HttpStatus.BeforeContent);
            // blocking receive operation
            int read =
                connection.Read(buffer, 0, 1);
            if (read == 0)  // connection closed by other endpoint
            {
                Status = HttpStatus.ConnectionError;
                return (char)0;
            }
            var c = (char)buffer[0];

            if (symbolLength < symbolBufferCapacity)
            {
                symbolBuffer[symbolLength] = c;
            }
            symbolLength = symbolLength + 1;

            return c;
        }

        /// <summary>
        /// Read a blank-terminated string from the socket.
        /// Postconditions
        ///     (s == null) <=> (Status > HttpReadStatus.InContent)
        /// If the reader is in an error state, this method does nothing.
        /// </summary>
        /// <param name="s">String that has been read, without the
        /// terminating blank character</param>
        public void ReadStringToBlank(out string s)
        {
            s = null;
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                symbolLength = 0;
                char c = ReadChar();
                while ((Status == HttpStatus.BeforeContent) &&
                       (c != ' ') && (c != '\r'))
                {
                    c = ReadChar();
                }
                if (Status == HttpStatus.BeforeContent)
                {
                    if (c == ' ')           // expected string terminator
                    {
                        if (symbolLength == 1)  // empty string not allowed
                        {
                            Status = HttpStatus.SyntaxError;
                        }
                        else if (symbolLength > symbolBufferCapacity)
                        // symbol too long
                        {
                            // not an error, but returned string is null
                        }
                        else
                        {
                            Contract.Assert(symbolLength > 1);
                            s = new string(symbolBuffer, 0,
                                            symbolLength - 1);
                        }
                    }
                    else
                    {
                        Status = HttpStatus.SyntaxError;
                    }
                }
            }
        }

        /// <summary>
        /// Read a colon-terminated string from the socket.
        /// The transition to reading the message content is signaled by
        /// returning the null string and by
        ///     (Status == HttpReadStatus.InContent).
        /// Postconditions
        ///     (s == null) => (Status > HttpReadStatus.InContent)
        /// If the reader is in an error state, this method does nothing.
        /// </summary>
        /// <param name="s">String that has been read, without the
        /// terminating colon</param>
        public void ReadFieldName(out string s)
        {
            s = null;
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                symbolLength = 0;
                char c = ReadChar();
                while ((Status == HttpStatus.BeforeContent) &&
                       (c != ':') && (c != '\r'))
                {
                    c = ReadChar();
                }
                if (Status == HttpStatus.BeforeContent)
                {
                    if (c == ':')           // expected string terminator
                    {
                        if (symbolLength == 1)  // empty symbol not allowed
                        {
                            Status = HttpStatus.SyntaxError;
                        }
                        else if (symbolLength > symbolBufferCapacity)
                        {   // symbol too long
                            Status = HttpStatus.SyntaxError;
                        }
                        else
                        {
                            Contract.Assert(symbolLength > 1);
                            s = new string(symbolBuffer, 0,
                                symbolLength - 1);
                        }
                    }
                    else    // not a string string, but end of headers
                    {
                        c = ReadChar();
                        if (c == '\n')
                        {
                            Status = HttpStatus.InContent;
                        }
                        else
                        {
                            Status = HttpStatus.SyntaxError;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Read a CR-LF-terminated string from the socket.
        /// Postconditions
        ///     (s == null) <=> (Status > HttpReadStatus.InContent)
        /// If the reader is in an error state, this method does nothing.
        /// </summary>
        /// <param name="s">String that has been read, without the
        /// terminating carriage return and newline characters</param>
        public void ReadFieldValue(out string s)
        {
            s = null;
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.BeforeContent);
                symbolLength = 0;
                char c = ReadChar();
                // skip optional white space
                while ((Status == HttpStatus.BeforeContent) && (c == ' '))
                {
                    symbolLength = 0;
                    c = ReadChar();
                }
                while ((Status == HttpStatus.BeforeContent) && (c != '\r'))
                {
                    c = ReadChar();
                }
                if (Status == HttpStatus.BeforeContent)
                {
                    c = ReadChar();
                    if (c == '\n')
                    {
                        if (symbolLength > symbolBufferCapacity)
                        {
                            // symbol too long is NOT considered as an
                            // error in a field value, the header will
                            // simply be skipped when the value is null
                        }
                        else
                        {
                            Contract.Assert(symbolLength >= 2);
                            s = new string(symbolBuffer, 0,
                                            symbolLength - 2);
                        }
                    }
                    else
                    {
                        Status = HttpStatus.SyntaxError;
                    }
                }
            }
        }

        /// <summary>
        /// Reads a number of bytes from the socket, as part of the message
        /// content. It cannot be used for reading request lines, status
        /// lines, or headers.
        /// If the reader is in an error state, this method does nothing.
        /// Preconditions
        ///     buffer != null
        ///     offset >= 0
        ///     count >= 0
        ///     offset + count lessEqual buffer.Length
        ///     "reader is attached"
        ///     (Status == HttpReadStatus.InContent) || "error"
        /// Postconditions
        ///     if "reader was in error state when method was called"
        ///         Result == -1
        ///         Status > HttpReadStatus.InContent
        ///     if "timeout occurred"
        ///         Result == -1
        ///         Status == HttpReadStatus.TimeoutError
        ///         IOException with message == "Timeout" was thrown
        ///     if "orderly shutdown of remote endpoint"
        ///         Result == -1
        ///         Status == HttpReadStatus.ConnectionError
        ///     if "no error occurred"
        ///         Result >= 0
        /// </summary>
        /// <param name="buffer">Some bytes from this buffer will be
        /// read</param>
        /// <param name="offset">Index of first byte to be read</param>
        /// <param name="count">Number of bytes to be read</param>
        /// <returns>Number of bytes actually read,
        /// or -1 for error</returns>
        public int ReadContent(byte[] buffer, int offset, int count)
        {
            Contract.Requires(buffer != null);
            Contract.Requires(offset >= 0);
            Contract.Requires(count >= 0);
            Contract.Requires(offset + count <= buffer.Length);
            Contract.Requires(connection != null);
            if (Status <= HttpStatus.InContent)    // no error
            {
                Contract.Requires(Status == HttpStatus.InContent);
                if (count == 0)
                {
                    return 0;
                }
                else
                {
                    // count > 0
                    int received = connection.Read(buffer, offset, count);
                    if (received == 0)  // shutdown of remote endpoint
                    {
                        Status = HttpStatus.ConnectionError;
                        return -1;
                    }
                    else
                    {
                        Contract.Assert(received > 0);
                        return received;
                    }
                }
            }
            else
            {
                return -1;
            }
        }

        /// <summary>
        /// Detaches an attached reader from a socket.
        /// It is NOT checked whether the correct number of content
        /// bytes were read! Even if the bytes are not processed,
        /// they MUST be read (consumed), otherwise they will be
        /// misinterpreted as part of the next request!
        /// Preconditions
        ///     "reader is attached"
        ///     Status == HttpReadStatus.InContent
        /// Postconditions
        ///     "reader is not attached"
        /// </summary>
        public void Detach()
        {
            Contract.Requires(connection != null);
            connection = null;
            // Don't change the Status property, it may still be
            // needed by the caller for error analysis.
        }
    }
}
