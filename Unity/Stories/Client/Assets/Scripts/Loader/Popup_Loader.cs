using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;
using System.Xml;

public class Popup_Loader : GameState
{
    public RectTransform[] loaders;
    public GameObject networkError = null;
    public GameObject serverMaintenance = null;

    public int downloadCount = 0;
    public bool isDownloading { get { return downloadCount > 0; } }

    public Popup_Loader DownloadAndCache(string url, int version, System.Action<WWW> callback)
    {
        downloadCount++;
        LoadFromCacheOrDownload(url, version, ws =>
        {
            if (ws.isDone == false || ws.bytesDownloaded < 1)
            {
                networkError.SetActive(true);
                DownloadAndCache(url, version, callback);
                downloadCount--;
                return;
            }
            else networkError.SetActive(false);

            if (ws.error.IsNullOrEmpty())
            {
                callback(ws);
                downloadCount--;
            }
            else serverMaintenance.SetActive(true);
        });
        return this;
    }

    public Popup_Loader DownloadXML(string url, System.Action<XmlReader> callback)
    {
        downloadCount++;
        Download(url, ws => PostDownload(ws, url, callback));
        return this;
    }

    private void PostDownload(WWW ws, string url, System.Action<XmlReader> callback)
    {
        if (ws.isDone == false || ws.bytesDownloaded < 1)
        {
            networkError.SetActive(true);
            DownloadXML(url, callback);
            downloadCount--;
            return;
        }
        else networkError.SetActive(false);

        var reader = Utilities.ParseXml(ws.text.GetWithoutBOM());
        if (reader != null)
        {
            callback(reader);
            downloadCount--;
        }
        else
        {
            serverMaintenance.SetActive(true);
            Debug.Log("Can't parse XML from " + url + "! Error message: " + Utilities.lastErrorMessage);
        }
    }

    public override void Back()
    {

    }

    public void Close()
    {
        base.Back();
    }

    private void Update()
    {
        for (int i = 0; i < loaders.Length; i++)
            loaders[i].Rotate(0, 0, Time.deltaTime * (i % 2 == 0 ? 100 : -150));
    }



    //////////////////////////////////////////////////////////////////////////////////
    //  static members
    //////////////////////////////////////////////////////////////////////////////////
    public static Popup_Loader XmlDownload(string url, System.Action<XmlReader> callback)
    {
        if (gameManager.CurrentPopup is Popup_Loader)
            return gameManager.CurrentPopup.As<Popup_Loader>().DownloadXML(url, callback);
        else
            return gameManager.OpenPopup<Popup_Loader>().DownloadXML(url, callback);
    }

    public static Popup_Loader FileDownloadAndCache(string url, int version, System.Action<WWW> callback)
    {
        if (gameManager.CurrentPopup is Popup_Loader)
            return gameManager.CurrentPopup.As<Popup_Loader>().DownloadAndCache(url, version, callback);
        else
            return gameManager.OpenPopup<Popup_Loader>().DownloadAndCache(url, version, callback);
    }
}
