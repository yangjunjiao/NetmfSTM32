using System;
using Microsoft.SPOT;
using Util;

namespace SimpleWebServer
{
    class HtmlHelloWorldData : IHtmlDataProvider
    {
        public string GetData()
        {
            return  "<bold>Hello World </bold><br>" + DateTime.Now.ToString();
        }
    }
}
