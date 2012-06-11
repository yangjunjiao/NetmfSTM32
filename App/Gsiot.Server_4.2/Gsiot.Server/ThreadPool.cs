using Gsiot.Contracts;
using Gsiot.Server;
using Microsoft.SPOT;

namespace System.Threading
{
    public delegate void WaitCallback(object state);

    internal class WorkItem
    {
        internal readonly WaitCallback callback;
        internal readonly object state;

        internal WorkItem(WaitCallback callback, object state)
        {
            Contract.Requires(callback != null);
            this.callback = callback;
            this.state = state;
        }
    }

    public static class ThreadPool
    {
        static Thread[] threads;
        static BoundedQueue workItemQueue;

        public static void Open(int nofThreads)
        {
            Contract.Requires(nofThreads > 0);
            threads = new Thread[nofThreads];
            var i = 0;
            while (i != nofThreads)
            {
                threads[i] = new Thread(Run);
                threads[i].Priority = ThreadPriority.Lowest;
                i = i + 1;
            }
            workItemQueue = new BoundedQueue { Capacity = nofThreads };
        }

        public static void Start()
        {
            var i = 0;
            while (i != threads.Length)
            {
                threads[i].Start();
                i = i + 1;
            }
        }

        public static bool QueueUserWorkItem(WaitCallback callback,
                                                object state)
        {
            Contract.Requires(callback != null);
            Contract.Requires(threads != null);
            Debug.Print("enqueue a work item");
            var item = new WorkItem(callback, state);
            workItemQueue.Enqueue(item);
            return true;
        }

        static void Run()
        {
            while (true)
            {
                Debug.Print("await work item");
                var obj = workItemQueue.Dequeue();      // may block!
                var item = (WorkItem)obj;
                Debug.Print("work item has been dequeued, process it");
                item.callback(item.state);
                Debug.Print("work item has been processed");
            }
        }
    }
}
