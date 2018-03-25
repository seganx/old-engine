using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace SeganX
{
    [CustomEditor(typeof(Localization2))]
    public class LocalizationEditor : Editor
    {
        public override void OnInspectorGUI()
        {
            var local = target.As<Localization2>();

            var cl = local.currnetLanguage;

            base.OnInspectorGUI();

            if (cl != local.currnetLanguage)
                local.OnValidation();
#if OFF
            var rect = GUILayoutUtility.GetRect(GUIContent.none, GUI.skin.button);
            var selectedLang = (LocalizationKit.Language)EditorGUI.EnumPopup(rect, new GUIContent("Currnet Language:"), Localization.CurrentLanguage);
            if (selectedLang != Localization.CurrentLanguage)
            {
                foreach (var item in Localization.all)
                    item.Save();

                Localization.CurrentLanguage = selectedLang;

                foreach (var item in Localization.all)
                    item.Load();

                var loctexts = FindObjectsOfType<LocalText>();
                foreach (var item in loctexts)
                    item.LoadTextFromlocalization();
            }
            base.OnInspectorGUI();

            GUILayout.Space(10);
            if (GUILayout.Button("Save Current Kit"))
            {
                target.As<Localization>().Save();
            }
#endif
        }
    }
}
