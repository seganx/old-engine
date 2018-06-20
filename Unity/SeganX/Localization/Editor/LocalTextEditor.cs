﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEditor;

namespace SeganX
{
    [CustomEditor(typeof(LocalText))]
    public class LocalTextEditor : Editor
    {
        private bool isSettingId = false;
        private int stringId = 0;

        public override void OnInspectorGUI()
        {
            var loctext = target.As<LocalText>();
            DrawItems(loctext);
        }

        public void DrawItems(LocalText local)
        {
            local.target = (Text)EditorGUILayout.ObjectField("Display Text:", local.target, typeof(Text), true);

            EditorGUILayout.Space();

            var rect = EditorGUILayout.GetControlRect();
            var maxWidth = rect.width;
            rect.width = 100;
            EditorGUI.PrefixLabel(rect, new GUIContent(local.stringId > 0 ? "Text: " + local.stringId : "Text: unlinked"));

            rect.width = 70;
            rect.x = maxWidth - rect.width;
            local.autoRtl = GUI.Toggle(rect, local.autoRtl, "Auto RTL", "Button");
            rect.width = 80;
            rect.x -= rect.width + 5;
            local.autoHeight = GUI.Toggle(rect, local.autoHeight, "Auto Height", "Button");
            rect.x -= rect.width + 5;
            local.autoWidth = GUI.Toggle(rect, local.autoWidth, "Auto Width", "Button");

            var curText = EditorGUILayout.TextArea(local.currnetText, new GUIStyle(GUI.skin.textArea) { wordWrap = true }, GUILayout.MinHeight(60));
            if (curText != local.currnetText)
            {
                local.SetText(curText.CleanFromCode().CleanForPersian());
                if (local.stringId > 0)
                    local.stringId = LocalizationService.UpdateString(local.stringId, local.currnetText);
            }

            rect = EditorGUILayout.GetControlRect();
            rect.width = 100;
            if (isSettingId == false)
            {

                if (GUI.Button(rect, "Set String Id"))
                {
                    isSettingId = true;
                    stringId = local.stringId;
                }

                rect.x = maxWidth - 100;
                if (GUI.Button(rect, "New Text"))
                    local.stringId = LocalizationService.UpdateString(0, local.currnetText);

                rect.width = 90;
                rect.x -= rect.width + 10;
                local.forcePersian = GUI.Toggle(rect, local.forcePersian, "Force Persian", "Button");

            }

            if (isSettingId)
            {
                stringId = EditorGUI.IntField(rect, stringId);
                rect.x = EditorGUIUtility.currentViewWidth - 108;
                if (GUI.Button(rect, "Apply"))
                {
                    isSettingId = false;
                    local.stringId = stringId;
                    local.Awake();
                }
            }
        }
    }
}
