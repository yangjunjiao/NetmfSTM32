using System;
//using Gsiot.Server;

public class HelloWebHtml
{
    public static void Main()
    {
        var webServer = new HttpServer
        {
            //RelayDomain = "<insert your relay domain here>",
            //RelaySecretKey = "<insert your secret key here>",
            RequestRouting =
            {
                { "GET /hello.html", HandleGetHelloHtml }
            }
        };
        webServer.Run();
    }

    static void HandleGetHelloHtml(RequestHandlerContext context)
    {
        string s =
            "<html>\r\n" +
            "\t<body>\r\n" +
            "\t\tHello <strong>Web</strong> at " +
                DateTime.Now + "\r\n" +
            "\t</body>\r\n" +
            "</html>";
        context.SetResponse(s, "text/html");
    }
}
