using UnityEngine;
using System.Collections;
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

    public void OnClick(string info)
    {
        Analytics.InGame_Tap(info);
    }

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

        if (res.error.HasContent())
        {
            Debug.Log("Failed to download from cache!\nDownloading from " + url);
            res = new WWW(url);
            yield return res;
            Debug.Log("Received bytes: " + res.bytesDownloaded);
            if (res.error.IsNullOrEmpty() && res.bytes.Length > 0)
                PlayerPrefsEx.SaveData(filename, res.bytes);
        }
        else Debug.Log("Loaded " + res.bytesDownloaded + " from cache");

        callback(res);
    }

    public void Download(string url, System.Action<WWW> callback)
    {
        StartCoroutine(DoDownload(url, callback));
    }

    IEnumerator DoDownload(string url, System.Action<WWW> callback)
    {
        Debug.Log("Downloading from " + url);
        var res = new WWW(url);
        yield return res;
        Debug.Log("Received bytes: " + res.bytesDownloaded);
        callback(res);
    }
}
