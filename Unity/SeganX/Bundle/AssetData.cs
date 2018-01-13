using System.Xml;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [CreateAssetMenu(menuName = "Game/AssetData")]
    public class AssetData : ScriptableObject
    {
        public int version = 1;
        public string type = "";
        public string tags = "";
        public List<AssetItem> prefabs = new List<AssetItem>();

        public bool HasTag(string[] taglist)
        {
            foreach (var item in taglist)
                if (tags.Contains(item))
                    return true;
            return false;
        }



        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        public static List<AssetData> all = new List<AssetData>();

        public static List<AssetData> FindByTags(string tags)
        {
            var res = new List<AssetData>();
            if (tags.HasContent())
            {
                var taglist = tags.Split(new char[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
                foreach (AssetData data in all)
                    if (data.HasTag(taglist))
                        res.Add(data);
            }
            return res;
        }

        public static AssetData LoadEncrypted(byte[] src)
        {
            var data = CryptoService.DecryptWithMac(src, Core.CryptoKey, Core.Salt);
            var bundle = AssetBundle.LoadFromMemory(data);
            if (bundle == null) return null;

            var path = bundle.GetAllAssetNames();
            foreach (var item in path)
            {
                var asset = bundle.LoadAsset<AssetData>(item);
                if (asset != null)
                {
                    all.Remove(asset);
                    all.Add(asset);
                    return asset;
                }
            }
            return null;
        }

        public static List<KeyValuePair<int, string>> LoadList(XmlReader reader)
        {
            var res = new List<KeyValuePair<int, string>>();
            while (reader.Read())
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "bundle")
                    res.Add(new KeyValuePair<int, string>(reader.GetAttribute("version").ToInt(0), reader.GetAttribute("link")));
            return res;
        }
    }
}
