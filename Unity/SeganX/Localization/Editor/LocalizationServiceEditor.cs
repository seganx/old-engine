using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace SeganX
{
    [CustomEditor(typeof(LocalizationService))]
    public class LocalizationServiceEditor : Editor
    {
        public override void OnInspectorGUI()
        {
            var local = target as LocalizationService;

            local.currentKit = (LocalizationKit)EditorGUILayout.ObjectField("Currnet Kit:", local.currentKit, typeof(LocalizationKit), false);

            if (local.currentKit == null)
            {
                EditorGUILayout.HelpBox("Please select a Kit file for localization service!", MessageType.Warning);
                return;
            }

            EditorGUILayout.Space();
            EditorGUILayout.Separator();
            EditorGUILayout.HelpBox("Current Kit file: " + local.currentKit.name, MessageType.None);

            LocalizationKitEditor.DrawKitItems(local.currentKit);
            EditorUtility.SetDirty(local.currentKit);

            EditorUtility.SetDirty(local);
        }
    }


    static class LocalizationMenu
    {
        [MenuItem("SeganX/Localization/Settings")]
        private static void Settings()
        {
            Selection.activeObject = LocalizationService.Instance;
        }
    }
}
