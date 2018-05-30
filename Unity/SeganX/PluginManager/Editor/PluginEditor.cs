﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

[CustomEditor(typeof(Plugin))]
public class PluginEditor : Editor
{
    private const float buttonWidth = 120;

    private GUIStyle style = null;

    public GUIStyle textStyle
    {
        get
        {
            if (style == null)
                style = new GUIStyle(GUI.skin.textField) { wordWrap = true };
            return style;
        }
    }

    public override void OnInspectorGUI()
    {
        var plugin = target as Plugin;
        var rect = EditorGUILayout.GetControlRect();
        float width = rect.width;

        if (plugin.folder.Length > 2)
        {
            EditorGUI.LabelField(rect, "Source folder: " + Path.GetFileName(plugin.folder));

            rect.width = buttonWidth;
            rect.x = width - rect.width;
            if (Directory.Exists(plugin.folder) && GUI.Button(rect, "Reveal in explorer"))
                EditorUtility.RevealInFinder(plugin.folder);

            EditorGUILayout.LabelField((plugin.activated ? "Activated " : "Deactivated ") + plugin.files.Count + " files ");
        }
        else
        {
            EditorGUI.LabelField(rect, "Source folder: NONE!");
            EditorGUILayout.Space();
            EditorGUILayout.HelpBox("Source folder is not valid!\nPlease set source folder in order to setup plugin.", MessageType.Error);
            rect = EditorGUILayout.GetControlRect();
            DisplaySetFolderButton(plugin, rect);
            return;
        }

        EditorGUILayout.Space();
        EditorGUILayout.LabelField("Scripting Define Symbols");
        plugin.symbols = EditorGUILayout.TextArea(plugin.symbols, GUILayout.MinHeight(30));

        EditorGUILayout.Space();
        EditorGUILayout.LabelField("Description");
        plugin.description = EditorGUILayout.TextArea(plugin.description, GUILayout.MinHeight(100));

        EditorGUILayout.Space();
        rect = EditorGUILayout.GetControlRect();
        rect.width = buttonWidth;
        DisplaySetFolderButton(plugin, rect);

        rect.x = width - rect.width;
        if (GUI.Button(rect, "Update source files"))
            OnUpdateSource(plugin);

        EditorGUILayout.Separator();
        rect = EditorGUILayout.GetControlRect();
        rect.width = buttonWidth;
        if (GUI.Button(rect, "Activate"))
            OnActivate(plugin);

        rect.x = width - rect.width;
        if (GUI.Button(rect, "Dectivate"))
            OnDeactivate(plugin);

        EditorUtility.SetDirty(target);
    }

    private void DisplaySetFolderButton(Plugin plugin, Rect rect)
    {
        if (GUI.Button(rect, "Set Folder"))
        {
            var folder = EditorUtility.OpenFolderPanel("Select plugin folder", plugin.folder, null);
            if (folder.HasContent(4))
            {
                plugin.folder = ValidateFolder(folder);
                OnSetFolder(plugin);
            }
        }
    }

    private static string ValidateFolder(string folder)
    {
        folder = folder.Replace("\n", "");
        if (folder.HasContent(3))
        {
            if (Directory.Exists(folder) && Path.GetFileName(folder).HasContent() == false)
                return folder.Remove(folder.Length - 1).MakeRelative(Application.dataPath);
            else
                return folder.MakeRelative(Application.dataPath);
        }
        return "";
    }

    private static void AddFilesAndFolderToList(List<string> list, string path)
    {
        if (Directory.Exists(path) == false) return;

        var files = Directory.GetFiles(path);
        foreach (var item in files)
            list.Add(item);

        var dirs = Directory.GetDirectories(path);
        foreach (var item in dirs)
        {
            list.Add(item);
            AddFilesAndFolderToList(list, item);
        }
    }

    private static void OnSetFolder(Plugin obj)
    {
        obj.files.Clear();
        AddFilesAndFolderToList(obj.files, obj.folder);
        for (int i = 0; i < obj.files.Count; i++)
            obj.files[i] = obj.files[i].Remove(0, obj.folder.Length);
    }

    private static void OnActivate(Plugin obj)
    {
        obj.activated = true;

        foreach (var item in obj.files)
            try
            {
                string src = (obj.folder + item).BuildPath(), dest = (Application.dataPath + item).BuildPath();
                if (Directory.Exists(src))
                {
                    if (!Directory.Exists(dest))
                        Directory.CreateDirectory(dest);
                }
                else try
                    {
                        if (File.Exists(dest))
                            File.Delete(dest);
                        if (File.Exists(src))
                            File.Copy(src, dest, true);
                    }
                    catch (System.Exception e)
                    {
                        Debug.LogWarning(e.Message);
                    }
            }
            catch { }

        AddRemoveSymbol(obj.symbols, false);

        EditorUtility.SetDirty(obj);
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

    private static void OnDeactivate(Plugin obj)
    {
        obj.activated = false;

        obj.files.Reverse();
        foreach (var item in obj.files)
            try
            {
                string path = (Application.dataPath + item).BuildPath();
                if (Directory.Exists(path))
                {
                    if (Directory.GetFiles(path).Length < 1)
                        try
                        {
                            Directory.Delete(path);
                            if (File.Exists(path + ".meta"))
                                File.Delete(path + ".meta");
                        }
                        catch (System.Exception e)
                        {
                            Debug.LogWarning(e.Message);
                        }
                }
                else if (File.Exists(path))
                    try
                    {
                        File.Delete(path);
                        if (File.Exists(path + ".meta"))
                            File.Delete(path + ".meta");
                    }
                    catch (System.Exception e)
                    {
                        Debug.LogWarning(e.Message);
                    }
            }
            catch { }
        obj.files.Reverse();

        AddRemoveSymbol(obj.symbols, true);

        EditorUtility.SetDirty(obj);
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

    public void OnUpdateSource(Plugin obj)
    {
        foreach (var item in obj.files)
            try
            {
                string src = (Application.dataPath + item).BuildPath(), dest = (obj.folder + item).BuildPath();
                if (Directory.Exists(src))
                {
                    if (!Directory.Exists(dest))
                        Directory.CreateDirectory(dest);
                }
                else try
                    {
                        if (File.Exists(dest))
                            File.Delete(dest);
                        if (File.Exists(src))
                            File.Copy(src, dest, true);
                    }
                    catch (System.Exception e)
                    {
                        Debug.LogWarning(e.Message);
                    }
            }
            catch { }
    }

    public static void AddRemoveSymbol(string symbol, bool remove)
    {
#if UNITY_ANDROID
        var buildTarget = BuildTargetGroup.Android;
#elif UNITY_IOS
        var buildTarget = BuildTargetGroup.iOS;
#elif UNITY_STANDALONE
        var buildTarget = BuildTargetGroup.Standalone;
#endif

        if (string.IsNullOrEmpty(symbol)) return;
        var symbols = symbol.Split(new char[] { ';' }, System.StringSplitOptions.RemoveEmptyEntries);
        var str = PlayerSettings.GetScriptingDefineSymbolsForGroup(buildTarget);
        if (remove)
            foreach (var item in symbols)
                str = str.Replace(item, "");
        else if (str.Contains(symbol) == false)
            str = str + ";" + string.Join(";", symbols);
        PlayerSettings.SetScriptingDefineSymbolsForGroup(buildTarget, str);
    }
}
