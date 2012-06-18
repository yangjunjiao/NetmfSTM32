using System;
using Gsiot.Contracts;
using Microsoft.SPOT;

namespace Gsiot.Server
{
    public class ServerDiagnostics
    {
        public TimeSpan ServiceUptime { get; set; }
        public int AcceptErrors { get; set; }
        public int AcceptFailures { get; set; }
        public int RequestsTotal { get; set; }
        public int RequestHandlerErrors { get; set; }
        public int RequestHandlerFailures { get; set; }
        public int AvailableHeapSpace { get; set; }
    }

    public class DiagnosticResource
    {
        public bool IsOpen { get; private set; }
        public ServerDiagnostics Diagnostics { get; set; }

        public DiagnosticResource()
        {
            IsOpen = false;
        }

        public void Open()
        {
            Contract.Requires(!IsOpen);
            IsOpen = true;
        }

        public void HandleRequest(RequestHandlerContext context)
        {
            Contract.Requires(context != null);
            if (!IsOpen) { Open(); }

            context.ResponseMaxAge = 0;         // no caching (default is -1, i.e., no cache header)
            var s = "";
            // Service Uptime
            TimeSpan delta = Diagnostics.ServiceUptime;
            s = s + "Service Uptime\t" + delta + "\r\n";
            // accept errors (e.g., timeouts)
            s = s + "Accept Errors\t" + Diagnostics.AcceptErrors + "\r\n";
            // accept failures (unknown exceptions)
            s = s + "Accept Failures\t" + Diagnostics.AcceptFailures + "\r\n";
            // requests total
            s = s + "Requests Total\t" + Diagnostics.RequestsTotal + "\r\n";
            // request handler errors (e.g., timeouts)
            s = s + "Request Handler Errors\t" + Diagnostics.RequestHandlerErrors + "\r\n";
            // request handler failures (unknown exceptions)
            s = s + "Request Handler Failures\t" + Diagnostics.RequestHandlerFailures + "\r\n";
            // free memory
            s = s + "Available Heap Space\t" + Debug.GC(true) + "\r\n";
            context.SetResponse(s, "text/plain");
        }
    }
}
