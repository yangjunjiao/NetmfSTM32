//Copyright 2012 Oberon microsystems, Inc.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

//Developed for the book
//  "Getting Started with the Internet of Things", by Cuno Pfister.
//  Copyright 2011 Cuno Pfister, Inc., 978-1-4493-9357-1.
//
// Version 1.0, for the .NET Micro Framework release 4.2.

// This file provides a mechanism for parallel actors to exchange data in
// a thread-safe way.

using System.Threading;
using System.Collections;
using Gsiot.Contracts;

namespace Gsiot.Server
{
    // only this one lock is used, so no deadlock may occur
    static class BigLock
    {
        internal readonly static object bigLock = new object();
    }

    /// <summary>
    /// The Buffer class provides a means for parallel actors to exchange
    /// data in a thread-safe way. One actor puts objects into a buffer
    /// instance, the other fetches them from the buffer.
    /// This buffer class is able to buffer at most one object at a time:
    /// if the producer puts a new object into the buffer, its old contents
    /// is replaced by the new one. When the consumer gets the current
    /// buffer contents, the buffer is not modified in any way.
    /// This buffer class is intended for actors that are synchronized to
    /// real-time ("time-triggered"); no mechanism for direct
    /// synchronization between the actors is provided.
    /// </summary>
    public sealed class Buffer
    {
        object buffer = null;

        /// <summary>
        /// Puts object o, which may be null, into the buffer.
        /// The current buffer contents is overwritten with the new one.
        /// It is safe to call this method from different threads.
        /// </summary>
        /// <param name="o">New value to be buffered.</param>
        public void HandlePut(object o)
        {
            lock (BigLock.bigLock)
            {
                buffer = o;
            }
        }

        /// <summary>
        /// Returns the object that is currently contained in the buffer,
        /// or null.
        /// It is safe to call this method from different threads.
        /// </summary>
        /// <returns>Currently buffered value. May be null.</returns>
        public object HandleGet()
        {
            object o;
            lock (BigLock.bigLock)
            {
                o = buffer;
            }
            return o;
        }
    }


    // TODO documentation
    public sealed class BoundedQueue
    {
        AutoResetEvent nonEmptySignal = new AutoResetEvent(false);
        AutoResetEvent nonFullSignal = new AutoResetEvent(false);

        int capacity = 0;           // where (capacity >= 0)
        Queue queue = new Queue();  // where (queue != null)

        public int Capacity         // once set, remains immutable
        {
            get { return capacity; }

            set
            {
                Contract.Requires(value > 0);
                capacity = value;
            }
        }

        public void Enqueue(object o)
        {
            Contract.Requires(capacity > 0);
            // this method may block
            int count;
            lock (BigLock.bigLock)
            {
                count = queue.Count;
                if (count == capacity)
                {
                    Monitor.Exit(BigLock.bigLock);
                    Microsoft.SPOT.Debug.Print("before wait one (enqueue)");
                    nonFullSignal.WaitOne();
                    Microsoft.SPOT.Debug.Print("after wait one (enqueue)");
                    Monitor.Enter(BigLock.bigLock);
                    Contract.Assert(queue.Count < capacity);
                }
                queue.Enqueue(o);
            }
            if (count == 0)
            {
                nonEmptySignal.Set();
            }
        }


        public object Dequeue()
        {
            Contract.Requires(capacity > 0);
            // this method may block
            object o = null;
            int count;
            lock (BigLock.bigLock)
            {
                count = queue.Count;
                if (count == 0)
                {
                    Monitor.Exit(BigLock.bigLock);
                    Microsoft.SPOT.Debug.Print("before wait one (dequeue)");
                    nonEmptySignal.WaitOne();
                    Microsoft.SPOT.Debug.Print("after wait one (dequeue)");
                    Monitor.Enter(BigLock.bigLock);
                    Contract.Assert(queue.Count > 0);
                }
                o = queue.Dequeue();
            }
            if (count == capacity)
            {
                nonFullSignal.Set();
            }
            return o;
        }
    }
}
