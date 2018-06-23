using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEditor;
using UnityEngine;
using SeganX;

public class AssetBundleBuilder
{
    [MenuItem("SeganX/AssetBundles/Build AssetBundles from Selection")]
    private static void BuildBundlesFromSelection()
    {
        // Get all selected *assets*
        var assets = Selection.objects.Where(o => !string.IsNullOrEmpty(AssetDatabase.GetAssetPath(o))).ToArray();

        List<AssetBundleBuild> assetBundleBuilds = new List<AssetBundleBuild>();
        HashSet<string> processedBundles = new HashSet<string>();

        // Get asset bundle names from selection
        foreach (var o in assets)
        {
            var assetPath = AssetDatabase.GetAssetPath(o);
            var importer = AssetImporter.GetAtPath(assetPath);
            if (importer == null) continue;

            // Get asset bundle name & variant
            var assetBundleName = importer.assetBundleName;
            var assetBundleVariant = importer.assetBundleVariant;
            var assetBundleFullName = string.IsNullOrEmpty(assetBundleVariant) ? assetBundleName : assetBundleName + "." + assetBundleVariant;

            // Only process assetBundleFullName once. No need to add it again.
            if (processedBundles.Contains(assetBundleFullName)) continue;
            processedBundles.Add(assetBundleFullName);

            AssetBundleBuild build = new AssetBundleBuild();

            build.assetBundleName = assetBundleName;
            build.assetBundleVariant = assetBundleVariant;
            build.assetNames = AssetDatabase.GetAssetPathsFromAssetBundle(assetBundleFullName);

            assetBundleBuilds.Add(build);
        }

        if (Core.Instance.assetBundleBuildOptions.android)
            BuildAssetBundles(assetBundleBuilds.ToArray(), BuildTarget.Android);
        if (Core.Instance.assetBundleBuildOptions.windows)
            BuildAssetBundles(assetBundleBuilds.ToArray(), BuildTarget.StandaloneWindows);
        if (Core.Instance.assetBundleBuildOptions.iOS)
            BuildAssetBundles(assetBundleBuilds.ToArray(), BuildTarget.iOS);
    }

    public static void BuildAssetBundles(AssetBundleBuild[] builds, BuildTarget target)
    {
        if (target == BuildTarget.iOS)
        {
#if ENABLE_IOS_ON_DEMAND_RESOURCES
                if (PlayerSettings.iOS.useOnDemandResources)
                    options |= BuildAssetBundleOptions.UncompressedAssetBundle;
#endif
#if ENABLE_IOS_APP_SLICING
                options |= BuildAssetBundleOptions.UncompressedAssetBundle;
#endif
        }

        AssetBundleManifest result = null;
        string outputPath = CreateAssetBundleDirectory(Core.Instance.assetBundleBuildOptions.outputPath, target);
        if (builds == null || builds.Length == 0)
            result = BuildPipeline.BuildAssetBundles(outputPath, Core.Instance.assetBundleBuildOptions.buildOptions, target);
        else
            result = BuildPipeline.BuildAssetBundles(outputPath, builds, Core.Instance.assetBundleBuildOptions.buildOptions, target);

        foreach (var item in result.GetAllAssetBundlesWithVariant())
            Debug.Log(item);
    }

    static public string CreateAssetBundleDirectory(string outputPath, BuildTarget target)
    {
        // Choose the output path according to the build target.
        string outputFolder = Path.Combine(outputPath, target.ToString());
        if (!Directory.Exists(outputFolder))
            Directory.CreateDirectory(outputFolder);
        return outputFolder;
    }
}
