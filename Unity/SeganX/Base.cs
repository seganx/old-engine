using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SeganX;

public class Base : MonoBehaviour
{
    public static Game gameManager { get { return GameManager.Instance as Game; } }
    public RectTransform rectTransform { get { return transform as RectTransform; } }
    public RectTransform parentRectTransform { get { return transform.parent as RectTransform; } }
    public virtual bool Visible { set { gameObject.SetActive(value); } get { return gameObject.activeSelf; } }

    public void DestroyGameObject(float delay)
    {
        Destroy(gameObject, delay);
    }

#if ANALYTICS
    public void OnAnalyticTap(string info)
    {
        Analytics.InGame_Tap(info);
    }
#endif

    public void DestroyScript(float delay)
    {
        Destroy(this, delay);
    }

    public void DelayCall(float seconds, System.Action callback)
    {
        StartCoroutine(DoDelayCall(seconds, callback));
    }

    IEnumerator DoDelayCall(float seconds, System.Action callback)
    {

        yield return new WaitForSeconds(seconds);
        callback();
    }

    public void LoadFromCacheOrDownload(string url, int version, System.Action<WWW> callback)
    {
        StartCoroutine(DoLoadFromCacheOrDownload(url, version, callback));
    }

    IEnumerator DoLoadFromCacheOrDownload(string url, int version, System.Action<WWW> callback)
    {
        var filename = url.ComputeMD5(Core.Salt + version) + ".seganx";
        var path = "file:///" + Application.persistentDataPath + "/" + filename;

        Debug.Log("Try loading from cache " + path);
        var res = new WWW(path);
        yield return res;

        if(res.error.HasContent())
        {
            Debug.Log("Failed to download from cache!\nDownloading from " + url);
            res = new WWW(url);
            yield return res;
            Debug.Log("Received bytes: " + res.bytesDownloaded);
            if(res.error.IsNullOrEmpty() && res.bytes.Length > 0)
                PlayerPrefsEx.SaveData(filename, res.bytes);
        }
        else
            Debug.Log("Loaded " + res.bytesDownloaded + " from cache");

        callback(res);
    }

    public void Download(string url, byte[] postData, Dictionary<string, string> header, System.Action<WWW> callback)
    {
        StartCoroutine(DoDownload(url, postData, header, callback));
    }


    IEnumerator DoDownload(string url, byte[] postData, Dictionary<string, string> httpheader, System.Action<WWW> callback)
    {
        WWW res = null;
        if(postData != null || httpheader != null)
        {
            if(httpheader == null)
                httpheader = new Dictionary<string, string>();

            if(httpheader.ContainsKey("Content-Type") == false)
                httpheader.Add("Content-Type", "application/json");

            if(postData != null)
                Debug.Log("Getting data from " + url + "\nHeader: " + httpheader.GetStringDebug() + "\nPostData:" + System.Text.Encoding.UTF8.GetString(postData));
            else
                Debug.Log("Getting data from " + url + "\nHeader: " + httpheader.GetStringDebug());

            res = new WWW(url, postData, httpheader);
        }
        else
        {
            Debug.Log("Getting data from " + url);
            res = new WWW(url);
        }

        yield return res;
        if(res.text.HasContent())
            Debug.Log("Received " + res.bytesDownloaded + " bytes from " + url + ":\n" + res.text);
        else
            Debug.Log("Received " + res.bytesDownloaded + " bytes from " + url);
        callback(res);
    }
}
