using UnityEngine;
using UnityEditor;
using System.IO;

public class EditorCacheItems
{
    [MenuItem("Tools/Cache/Clear All")]
    private static void ClearAll()
    {
        ClearPlayerPrefs();
        ClearFiles();
    }

    [MenuItem("Tools/Cache/Clear PlayerPrefs")]
    private static void ClearPlayerPrefs()
    {
        PlayerPrefs.DeleteAll();
    }

    [MenuItem("Tools/Cache/Clear Files")]
    private static void ClearFiles()
    {
        var files = Directory.GetFiles(Application.temporaryCachePath);
        foreach (var item in files)
            try { File.Delete(item); }
            catch { };

        var dirs = Directory.GetDirectories(Application.temporaryCachePath);
        foreach (var item in dirs)
            try { Directory.Delete(item, true); }
            catch { };
    }
}
