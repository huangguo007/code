using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace IRIQCapture
{
    public class ThreadsManager : IDisposable
    {
        public ThreadsManager()
        {
            IsThreadStop = false;
        }

        public void ThreadStart(ThreadStart threadStart)
        {
            WorkerThread = new Thread(threadStart);
            WorkerThread.Start();
            // Loop until worker thread activates.
            while (!WorkerThread.IsAlive) ;
            // Put the main thread to sleep for 1 millisecond to
            // allow the worker thread to do some work:
            Thread.Sleep(1);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }

        public volatile bool IsThreadStop;
        public Thread WorkerThread;

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                IsThreadStop = true;
            }
        }
    }
}
