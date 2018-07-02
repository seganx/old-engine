using System.Xml;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [CreateAssetMenu(menuName = "Game/AssetData")]
    public class AssetData : ScriptableObject
    {
        public int id = 0;
        public string type = "";
        [PersianPreview]
        public string tags = "";
        public List<AssetItem> prefabs = new List<AssetItem>();


        public bool HasTag(string[] taglist)
        {
            foreach (var item in taglist)
                if (tags.Contains(item))
                    return true;
            return false;
        }

        public override string ToString()
        {
            return id + " : " + type;
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

        public static AssetData LoadEncrypted(int id, byte[] src)
        {
            //  search to see if the asset bundle is already loaded
            {
                var loadedOne = all.Find(x => x.id == id);
                if (loadedOne != null) return loadedOne;
            }

            var data = CryptoService.DecryptWithMac(src, Core.CryptoKey, Core.Salt);
            if (data == null) return null;

            AssetBundle bundle = null;
            try
            {
                bundle = AssetBundle.LoadFromMemory(data);
            }
            catch (System.Exception e)
            {
                Debug.LogError("Can't load asset bundle " + id + " du to error: " + e.Message);
            }
            if (bundle == null) return null;

            var path = bundle.GetAllAssetNames();
            foreach (var item in path)
            {
                var asset = bundle.LoadAsset<AssetData>(item);
                if (asset != null)
                {
                    all.Remove(asset);
                    all.Add(asset);

                    if (id > 0)
                        asset.id = id;

                    return asset;
                }
            }
            return null;
        }
    }
}
