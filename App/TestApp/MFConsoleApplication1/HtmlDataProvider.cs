

namespace MFConsoleApplication1
{
    using System;
    using Microsoft.SPOT;

    class HtmlTempDataProvider : IHtmlDataProvider
    {
        private Ds1624 tempSensor;
        public HtmlTempDataProvider(Ds1624 tempSensor)
        {
            this.tempSensor = tempSensor;
        }
        public string GetData()
        {
            return tempSensor.LastTemperature.ToString("F2") + "°C";
        }
    }
}
