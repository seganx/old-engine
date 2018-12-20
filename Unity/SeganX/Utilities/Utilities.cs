using System;
using System.IO;
using System.Xml;


namespace SeganX
{
    public static class Utilities
    {
        public static string lastErrorMessage = "";

        public static XmlReader ParseXml(string xmlText)
        {
            if (string.IsNullOrEmpty(xmlText))
            {
                lastErrorMessage = "Can't parse empty text!";
                return null;
            }

            //  check to see if XML is valid
            var document = new XmlDocument();
            try { document.LoadXml(xmlText); }
            catch (XmlException e)
            {
                lastErrorMessage = e.Message;
                return null;
            }

            try
            {
                XmlTextReader reader = new XmlTextReader(new StringReader(xmlText));
                reader.WhitespaceHandling = WhitespaceHandling.None;
                return reader;
            }
            catch (Exception e)
            {
                lastErrorMessage = e.Message;
                return null;
            }
        }

        public static DateTime UnixTimeToLocalTime(string date)
        {
            DateTime res = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
            res = res.AddSeconds(double.Parse(date));
            return res.ToLocalTime();
        }

        public static DateTime UnixTimeToLocalTime(long date)
        {
            DateTime res = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
            res = res.AddSeconds(date);
            return res.ToLocalTime();
        }

        public static string TimeToString(TimeSpan span)
        {
            if (span.Days > 0)
                return span.Days + ":" + span.Hours + ":" + span.Minutes + ":" + span.Seconds;
            else if (span.Hours > 0)
                return span.Hours + ":" + span.Minutes + ":" + span.Seconds;
            else
                return span.Minutes + ":" + span.Seconds;
        }
    }
}
