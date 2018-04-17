using System.Xml;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [CreateAssetMenu(menuName = "Game/AssetData")]
    public class AssetData : ScriptableObject
    {
        [HideInInspector]
        public string uri = "";
        [InspectorButton(100, "Generate Id", "GenerateId")]
        public int id = 0;
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

#if UNITY_EDITOR
        public void GenerateId(object sender)
        {
            id = EditorOnlineData.GenerateAssetId();
            UnityEditor.EditorUtility.SetDirty(this);
        }
#endif

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

        public static AssetData LoadEncrypted(string uri, byte[] src)
        {
            var data = CryptoService.DecryptWithMac(src, Core.CryptoKey, Core.Salt);
            if (data == null) return null;

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
                    asset.uri = uri;
                    return asset;
                }
            }
            return null;
        }
    }
}
