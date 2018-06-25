using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEditor;
using UnityEngine;
using SeganX;

public class AssetBundleBuilder
{
    public class BuildTextureData
    {
        public string path = "";
        public int maxSize = 256;
    }

    [MenuItem("SeganX/AssetBundles/Build from Selection")]
    public static void BuildBundlesFromSelection()
    {
        // get all selected *assets*
        var selectedAssets = Selection.objects.Where(o => !string.IsNullOrEmpty(AssetDatabase.GetAssetPath(o))).ToArray();
        if (selectedAssets.Length < 1) return;

        List<AssetBundleBuild> assetBundleBuilds = new List<AssetBundleBuild>();
        HashSet<string> processedBundles = new HashSet<string>();

        // get asset bundle names from selection
        foreach (var serlectedOne in selectedAssets)
        {
            var assetPath = AssetDatabase.GetAssetPath(serlectedOne);
            var importer = AssetImporter.GetAtPath(assetPath);
            if (importer == null) continue;

            // get asset bundle name & variant
            var assetBundleName = importer.assetBundleName;
            var assetBundleVariant = importer.assetBundleVariant;
            var assetBundleFullName = string.IsNullOrEmpty(assetBundleVariant) ? assetBundleName : assetBundleName + "." + assetBundleVariant;

            // gnly process assetBundleFullName once. No need to add it again.
            if (processedBundles.Contains(assetBundleFullName)) continue;
            processedBundles.Add(assetBundleFullName);

            AssetBundleBuild build = new AssetBundleBuild();

            build.assetBundleName = assetBundleName;
            build.assetBundleVariant = assetBundleVariant;
            build.assetNames = AssetDatabase.GetAssetPathsFromAssetBundle(assetBundleFullName);

            assetBundleBuilds.Add(build);
        }

        BuildAssetBundlesAllTargets(assetBundleBuilds.ToArray());
    }

    [MenuItem("SeganX/AssetBundles/Build All")]
    public static void BuildAllAssetBundles()
    {
        BuildAssetBundlesAllTargets(null);
    }

    public static void BuildAssetBundlesAllTargets(AssetBundleBuild[] builds)
    {
        foreach (var buildOption in Core.Instance.assetBundleBuildOptions.builds)
        {
            if (buildOption.active == false) continue;

            var outputPath = VerifyDirectory(Core.Instance.assetBundleBuildOptions.outputPath, buildOption.folder);
            if (builds == null)
                VerifyTexturesInPath(Core.Instance.assetBundleBuildOptions.bundlesPath, (int)buildOption.textureSize, () => BuildAssetBundles(null, buildOption.platform, outputPath, buildOption.suffix, buildOption.encrypt));
            else
                VerifyTexturesInBuilds(builds, (int)buildOption.textureSize, () => BuildAssetBundles(builds, buildOption.platform, outputPath, buildOption.suffix, buildOption.encrypt));
        }
    }

    public static void BuildAssetBundles(AssetBundleBuild[] builds, BuildTarget target, string outputPath, string suffix, bool encrypt)
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
        if (builds == null || builds.Length == 0)
            result = BuildPipeline.BuildAssetBundles(outputPath, Core.Instance.assetBundleBuildOptions.buildOptions, target);
        else
            result = BuildPipeline.BuildAssetBundles(outputPath, builds, Core.Instance.assetBundleBuildOptions.buildOptions, target);

        // list and encrypt assets if needed
        foreach (var item in result.GetAllAssetBundlesWithVariant())
        {
            var path = Path.Combine(outputPath, item);
            try
            {
                var src = File.ReadAllBytes(path);
                var data = encrypt ? CryptoService.EncryptWithMac(src, Core.CryptoKey, Core.Salt) : src;
                File.WriteAllBytes(path + suffix + ".seganx", data);
                Debug.Log("Built: " + path + suffix + ".seganx");
                File.Delete(path);
                File.Delete(path + ".manifest");
            }
            catch (System.Exception e)
            {
                Debug.LogError(e.Message);
            }
        }

    }

    static public string VerifyDirectory(string outputPath, string folder)
    {
        string outputFolder = Path.Combine(outputPath, folder);
        if (!Directory.Exists(outputFolder))
            Directory.CreateDirectory(outputFolder);
        return outputFolder;
    }

    static public void VerifyTexturesInBuilds(AssetBundleBuild[] builds, int sizefactor, System.Action callback)
    {
        if (sizefactor == 1)
        {
            callback();
            return;
        }

        var textureList = new List<BuildTextureData>();

        //  collect all textures in dependencies
        foreach (var build in builds)
            foreach (var asset in build.assetNames)
                foreach (var item in AssetDatabase.GetDependencies(asset))
                    if (AssetImporter.GetAtPath(item) is TextureImporter)
                        textureList.Add(new BuildTextureData() { path = item });

        VerifyTexturesSize(textureList, sizefactor, callback);
    }

    static public void VerifyTexturesInPath(string path, int sizefactor, System.Action callback)
    {
        if (sizefactor == 1)
        {
            callback();
            return;
        }

        var textureList = new List<BuildTextureData>();
        var guids = AssetDatabase.FindAssets("t:texture2D", new string[] { path });
        foreach (var guid in guids)
            textureList.Add(new BuildTextureData() { path = AssetDatabase.GUIDToAssetPath(guid) });

        VerifyTexturesSize(textureList, sizefactor, callback);
    }

    static public void VerifyTexturesSize(List<BuildTextureData> textureList, int sizefactor, System.Action callback)
    {
        //  verify texture size
        foreach (var texture in textureList)
        {
            var importer = TextureImporter.GetAtPath(texture.path) as TextureImporter;
            texture.maxSize = importer.maxTextureSize;
            importer.maxTextureSize /= sizefactor;
            AssetDatabase.ImportAsset(texture.path, ImportAssetOptions.ForceUpdate);
        }

        callback();

        foreach (var texture in textureList)
        {
            var importer = TextureImporter.GetAtPath(texture.path) as TextureImporter;
            importer.maxTextureSize = texture.maxSize;
            AssetDatabase.ImportAsset(texture.path, ImportAssetOptions.ForceUpdate);
        }
    }
}
