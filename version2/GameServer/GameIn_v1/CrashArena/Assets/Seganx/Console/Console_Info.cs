using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class Console_Info : MonoBehaviour
{
    public delegate string OnDisplayInfoEvent(string info);
    public static OnDisplayInfoEvent OnDisplayInfo = null;

    public Text label = null;

    public string DisplayDeviceID
    {
        get
        {
            string res = "";
            string str = Core.DeviceId;
            for (int i = 0; i < str.Length; i++)
            {
                if (i > 0 && i % 4 == 0)
                    res += " ";
                res += str[i];
            }
            return res;
        }
    }

    void OnEnable()
    {
        if (gameObject.activeInHierarchy == false) return;
        Invoke("OnEnable", 0.5f);

        string str = "Version: " + Application.version + " - DeviceId: " + DisplayDeviceID;
        if (OnDisplayInfo != null)
            str = OnDisplayInfo(str);
        //str += "\n" + System.DateTime.Now + " - Server: " + Network.Instance.ServerType + " - Nickname: " + Game.Instance.player.NickName;

        label.text = str;
    }
}
