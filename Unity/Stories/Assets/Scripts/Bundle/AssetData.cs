using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class AssetData : ScriptableObject
    {
        public int version = 1;

        public static AssetData Load(WWW ws)
        {
            if (ws.assetBundle != null)
            {
                var bundle = ws.assetBundle;
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
            }
            return null;
        }

        public static T Find<T>(string name) where T : AssetData
        {
            var res = s_assets.Find(x => x.name == name);
            return res != null ? res as T : null;
        }

        public static List<AssetData> s_assets = new List<AssetData>();
    }
}
