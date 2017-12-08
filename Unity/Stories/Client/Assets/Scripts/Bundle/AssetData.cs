using System.Xml;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class AssetData : ScriptableObject
    {
        public int version = 1;

        public static AssetData LoadEncrypted(byte[] src)
        {
            var data = CryptoService.DecryptWithMac(src, Core.CryptoKey, Core.Salt);
            var bundle = AssetBundle.LoadFromMemory(data);
            var path = bundle.GetAllAssetNames();
            foreach (var item in path)
            {
                var asset = bundle.LoadAsset<AssetData>(item);
                if (asset != null)
                {
                    s_assets.Remove(asset);
                    s_assets.Add(asset);
                    return asset;
                }
            }
            return null;
        }

        public static T Find<T>(string name) where T : AssetData
        {
            var res = s_assets.Find(x => x.name == name);
            return res != null ? res as T : null;
        }

        public static List<KeyValuePair<int, string>> LoadList(XmlReader reader)
        {
            var res = new List<KeyValuePair<int, string>>();
            while (reader.Read())
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "bundle")
                    res.Add(new KeyValuePair<int, string>(reader.GetAttribute("version").ToInt(0), reader.GetAttribute("link")));
            return res;
        }

        public static List<AssetData> s_assets = new List<AssetData>();
    }
}
