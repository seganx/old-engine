using UnityEngine;
using System.Collections;
using SeganX;

public class Base : MonoBehaviour
{
    public Game game { get { return Game.Instance; } }

    public virtual bool Visible { set { gameObject.SetActive(value); } get { return gameObject.activeSelf; } }

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
        var path = "file:///" + Application.temporaryCachePath + "/" + filename;

        var res = new WWW(path);
        yield return res;

        if (res.error.HasContent())
        {
            res = new WWW(url);
            yield return res;

            if (res.error.IsNullOrEmpty() && res.bytes.Length > 0)
                PlayerPrefsEx.SaveData(filename, res.bytes);
        }

        callback(res);
    }

    public void Download(string url, System.Action<WWW> callback)
    {
        StartCoroutine(DoDownload(url, callback));
    }

    IEnumerator DoDownload(string url, System.Action<WWW> callback)
    {
        var res = new WWW(url);
        yield return res;
        callback(res);
    }
}
