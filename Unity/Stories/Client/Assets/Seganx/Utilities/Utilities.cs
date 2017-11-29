using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Xml;
using System.IO;

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
            catch (System.Xml.XmlException e)
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
            catch (System.Exception e)
            {
                lastErrorMessage = e.Message;
                return null;
            }
        }
    }
}
