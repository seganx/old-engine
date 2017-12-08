using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class Game : GameManager
{
    [Header("Config:")]
    public string version = "1.1";
    public string locatorAddress = "http://locator.8khan.ir/Tests/Sajad/";

    [Header("Defaults:")]
    public Sprite defaultEmptySprite = null;
    public Sprite defaultPlayerSprite = null;

    public string baseAddress { get { return locatorAddress + version + "/"; } }

    IEnumerator Start()
    {
        //   load config
        var loader = Popup_Loader.XmlDownload(baseAddress + "config.xml", reader => { });
        while (loader.isDownloading)
            yield return new WaitForEndOfFrame();

        // load assets
        loader.DownloadXML(baseAddress + "bundles/bundles.xml", xmlReader =>
        {
            var list = AssetData.LoadList(xmlReader);
            foreach (var item in list)
                loader.DownloadAndCache(item.Value, item.Key, ws => AssetData.LoadEncrypted(ws.bytes));
        });
        while (loader.isDownloading)
            yield return new WaitForEndOfFrame();

        //  load books
        loader.DownloadXML(baseAddress + "books.xml", xmlReader =>
        {
            var list = BookData.LoadList(xmlReader);
            foreach (var item in list)
                loader.DownloadXML(baseAddress + item, reader => BookData.Load(reader));
        });
        while (loader.isDownloading)
            yield return new WaitForEndOfFrame();

        loader.Close();

        OpenState<State_StoryEditor>();
    }
}
