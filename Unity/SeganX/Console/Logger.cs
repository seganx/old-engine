using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

namespace SeganX.Console
{
    public class Logger : MonoBehaviour
    {
        public class LogText
        {
            public int repeated = 1;
            public Color color = Color.white;
            public string text = string.Empty;
            public float top = 0;
            public float height = 0;
            public Text visual = null;
            public string visualText { get { return repeated < 2 ? text : "(" + repeated + ") " + text; } }
        }

        public int maxTextCount = 2000;
        public int maxTextLength = 16000;
        public ScrollRect scroll = null;
        public Toggle autoScroll = null;

        private Text textVisual = null;
        private List<LogText> textList = new List<LogText>();
        private volatile List<KeyValuePair<string, Color>> threadedList = new List<KeyValuePair<string, Color>>();

        void Awake()
        {
            instance = this;
            if (SystemInfo.systemMemorySize < 1000)
            {
                maxTextCount = 500;
                maxTextLength = 2000;
            }

            textVisual = scroll.content.GetChild<Text>(0);
            textVisual.text = string.Empty;
            GetComponent<Canvas>().enabled = true;
            gameObject.SetActive(enable = false);
            
            Application.logMessageReceivedThreaded += HandleLog;
        }

        void OnEnable()
        {
            if (Enabled)
                Invoke("OnEnable", 0.05f);

            foreach (var item in textList)
            {
                float topEdge = item.top + scroll.content.anchoredPosition.y;
                float botEdge = topEdge - item.height;
                if (topEdge >= -scroll.viewport.rect.height && botEdge <= 0)
                {
                    if (item.visual == null)
                    {
                        item.visual = textVisual.gameObject.Clone<Text>();
                        item.visual.rectTransform.SetAnchordPositionY(item.top);
                        item.visual.rectTransform.SetAnchordHeight(item.height);
                        item.visual.color = item.color;
                        item.visual.text = item.visualText;
                    }
                }
                else if (item.visual != null)
                {
                    item.visual.gameObject.DestroyNow();
                    item.visual = null;
                }
            }

            if (autoScroll.isOn)
                scroll.normalizedPosition = Vector2.zero;
        }

        void HandleLog(string condition, string stacktrace, LogType type)
        {
            if (!Enabled || condition.IsNullOrEmpty()) return;

            lock (threadedList)
            {
                switch (type)
                {
                    case LogType.Assert:
                    case LogType.Warning:
                        threadedList.Add(new KeyValuePair<string, Color>(condition + "\n" + stacktrace, Color.yellow));
                        break;

                    case LogType.Error:
                    case LogType.Exception:
                        threadedList.Add(new KeyValuePair<string, Color>(condition + "\n" + stacktrace, Color.red));
                        break;

                    default: threadedList.Add(new KeyValuePair<string, Color>(condition, Color.white));
                        break;
                }
            }
        }

        void AddToLog(string str, Color color)
        {
            if (str.Length > maxTextLength)
            {
                int index = str.Length - maxTextLength;
                AddToLog(str.Remove(index), color);
                str = str.Substring(index);
            }

            var lastText = textList.Count > 0 ? textList[textList.Count - 1] : null;
            if (lastText != null && lastText.text == str)
            {
                lastText.repeated++;
                if (lastText.visual)
                {
                    lastText.visual.text = lastText.visualText;
                    lastText.height = lastText.visual.flexibleHeight;
                }
            }
            else
            {
                var logtext = new LogText();
                logtext.text = str;
                logtext.color = color;

                textVisual.text = str;
                logtext.height = textVisual.preferredHeight;
                textVisual.text = string.Empty;

                textList.Add(logtext);
                scroll.content.SetAnchordHeight(ComputeHeight(textList));
            }
        }

        public void Clear()
        {
            foreach (var item in textList)
                if (item.visual != null)
                    item.visual.gameObject.DestroyNow();
            textList.Clear();
            scroll.content.transform.SetAnchordHeight(10);
            scroll.content.SetAnchordPositionY(0);
        }

        private void Start()
        {
            AddToLog("Start Version " + Application.version + " on " + Core.DeviceId + " based on " + Core.BaseDeviceId, Color.green);
        }

        private void LateUpdate()
        {
            lock (threadedList)
            {
                if (threadedList.Count > 0)
                {
                    var item = threadedList[0];
                    threadedList.RemoveAt(0);
                    AddToLog(item.Key, item.Value);
                }
            }
        }


        /////////////////////////////////////////////////////////////////////////////
        //  STATICS
        /////////////////////////////////////////////////////////////////////////////
        private static Logger instance = null;
        private static bool enable = false;

        public static bool Enabled
        {
            get { return enable; }
            set { instance.gameObject.SetActive(enable = value); }
        }

        private static float ComputeHeight(List<LogText> list)
        {
            float height = 0;
            foreach (var item in list)
            {
                item.top = -height;
                height += item.height;
            }
            return height;
        }

        [Console("Save", "Log")]
        public static void SaveLog()
        {
            string str = string.Empty;
            foreach (var item in instance.textList)
                str += item.visualText + "\n";

            var filename = Application.temporaryCachePath + ".txt";
            System.IO.File.WriteAllText(filename, str.Replace("\n", "\r\n"));
            Debug.Log("Saved to " + filename);

            Application.OpenURL(filename);
        }

        [Console("Clear", "Cache")]
        public static void ClearCache()
        {
            PlayerPrefs.DeleteAll();
            Caching.ClearCache();
            Debug.Log("Cache Cleared");
        }

        [Console("Clear", "Data")]
        public static void ClearData()
        {
            ClearCache();
            PlayerPrefsEx.ClearData();
            Debug.Log("Data Cleared");
        }


        [Console("Path", "Data")]
        public static void PathData()
        {
            Debug.Log(Application.persistentDataPath);
        }

        [Console("Path", "Cache")]
        public static void PathCache()
        {
            Debug.Log(Application.temporaryCachePath);
        }
    }
}