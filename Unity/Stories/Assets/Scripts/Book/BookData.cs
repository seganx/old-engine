using System.Collections.Generic;
using System.Xml;
using SeganX;

public class BookData
{
    public int Id = 0;
    public int chapter = 0;
    public string name = "";
    public string chapterName = "";
    public string author = "";
    public string description = "";
    public string release = "";
    public string imageThumbnailUri = "";
    public string imagePreviewUri = "";
    public string imageLoadingUri = "";
    public List<string> bundles = new List<string>();


    public static List<BookData> all = new List<BookData>();

    public static bool Load(XmlReader reader)
    {
        BookData item = null;
        string elementName = "";
        while (reader.Read())
        {
            switch (reader.NodeType)
            {
                case XmlNodeType.Element:
                    elementName = reader.Name;
                    if (reader.Name == "book")
                    {
                        item = new BookData();
                        item.Id = reader.GetAttribute("id").ToInt(0);
                        item.chapter = reader.GetAttribute("chapter").ToInt(0);
                    }
                    break;

                case XmlNodeType.Text:
                    switch (elementName)
                    {
                        case "name": item.name = reader.Value; break;
                        case "chapter": item.chapterName = reader.Value.Replace("_", "\n"); break;
                        case "auther": item.author = reader.Value; break;
                        case "desc": item.description = reader.Value; break;
                        case "release": item.release = reader.Value; break;
                        case "bundle": item.bundles.Add(reader.Value); break;
                    }
                    break;

                case XmlNodeType.EndElement:
                    elementName = null;
                    if (reader.Name == "book")
                        all.Add(item);
                    break;
            }
        }

        return true;
    }

    public static List<string> LoadList(XmlReader reader)
    {
        var res = new List<string>();

        while (reader.Read())
            if (reader.NodeType == XmlNodeType.Element && reader.Name == "book")
                res.Add(reader.GetAttribute("link") + "Book.xml");

        return res;
    }
}

