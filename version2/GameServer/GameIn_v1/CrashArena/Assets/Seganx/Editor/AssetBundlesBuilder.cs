﻿#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;

namespace SeganX.Editor
{
    public class AssetBundlesBuilder
    {
        [MenuItem("SeganX/AssetBundles/Build")]
        private static void BuildAllAssetBundles()
        {
            BuildPipeline.BuildAssetBundles("AssetBundles", BuildAssetBundleOptions.None, BuildTarget.Android);
        }

    }
}
#endif