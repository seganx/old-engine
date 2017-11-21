#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;

namespace SeganX.Editor
{
    public class AssetBoundleBuilder
    {
        [MenuItem("SeganX/AssetBundles/Build")]
        private static void BuildAllAssetBundles()
        {
            var path = EditorUtility.SaveFolderPanel("Save Asset Boundle", null, null);
            BuildPipeline.BuildAssetBundles(path, BuildAssetBundleOptions.None, BuildTarget.Android);
        }

    }
}
#endif