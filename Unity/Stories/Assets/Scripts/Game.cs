using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class Game : GameManager
{
    [Header("Config:")]
    public string version = "1.1";
    public string locatorAddress = "http://locator.8khan.ir/Tests/Sajad/";

    public string baseAddress { get { return locatorAddress + version + "/"; } }

    IEnumerator Start()
    {
        var loader = OpenPopup<Popup_Loader>();

        //   load config
        loader.DownloadXML(baseAddress + "Config.xml", reader => { });
        while (loader.isDownloading)
            yield return null;

        //  load books
        loader.DownloadXML(baseAddress + "Books.xml", xmlReader =>
        {
            var list = BookData.LoadList(xmlReader);
            foreach (var item in list)
                loader.DownloadXML(baseAddress + item, reader => BookData.Load(reader));
        });
        while (loader.isDownloading)
            yield return null;

        Debug.Log(BookData.all.GetStringDebug());
    }
}
