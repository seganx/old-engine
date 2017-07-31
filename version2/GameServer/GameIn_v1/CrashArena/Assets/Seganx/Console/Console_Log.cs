using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

public class Console_Log : MonoBehaviour
{
    public class LogText
    {
        public int repeated = 1;
        public Color color = Color.white;
        public string text = "";
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

    void Awake()
    {
        instance = this;
        if (SystemInfo.systemMemorySize < 1000)
        {
            maxTextCount = 500;
            maxTextLength = 2000;
        }

        textVisual = scroll.content.GetChild<Text>(0);
        textVisual.text = "";
        GetComponent<Canvas>().enabled = true;
        //gameObject.SetActive(false);


        Application.logMessageReceivedThreaded += HandleLog;
        AddToLog("Start Version " + Application.version + " on " + Core.DeviceId + " based on " + Core.BaseDeviceId, Color.green);
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
        if (!Enabled || condition.IsNullOrEmpty() || condition.Length < 5) return;

        switch (type)
        {
            case LogType.Assert:
            case LogType.Warning: AddToLog(condition + "\n" + stacktrace, Color.yellow); break;
            case LogType.Error:
            case LogType.Exception: AddToLog(condition + "\n" + stacktrace, Color.red); break;
            default: AddToLog(condition, Color.white); break;
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
                lastText.height = lastText.visual.GetTextHeight(lastText.visualText);
                lastText.visual.text = lastText.visualText;
            }
        }
        else
        {
            var logtext = CreateLogText(textVisual, str, color);
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


    /////////////////////////////////////////////////////////////////////////////
    //  STATICS
    /////////////////////////////////////////////////////////////////////////////
    private static Console_Log instance = null;

    public static bool Enabled
    {
        get { return instance.gameObject.activeInHierarchy; }
        set { instance.gameObject.SetActive(value); }
    }

    private static LogText CreateLogText(Text visual, string str, Color color)
    {
        var result = new LogText();
        result.text = str;
        result.color = color;
        result.height = visual.GetTextHeight(str);
        return result;
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

}
