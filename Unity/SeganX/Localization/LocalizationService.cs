﻿using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace SeganX
{
    public class LocalizationService : ScriptableObject
    {
        public int baseId = 111000;
        public LocalizationKit currentKit = null;

#if UNITY_EDITOR
        private void OnEnable()
        {
            instance = this;
        }

        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        public static List<LocalizationKit> kits = new List<LocalizationKit>();
        private static LocalizationService instance = null;
        public static LocalizationService Instance { get { CheckService(); return instance; } }

        public static void CheckService()
        {
            var path = "/Resources/Localization/";
            var fileName = path + "LocalizationService.asset";
            if (File.Exists(Application.dataPath + fileName)) return;

            var ioPath = Application.dataPath + path;
            if (!Directory.Exists(ioPath)) Directory.CreateDirectory(ioPath);

            instance = ScriptableObject.CreateInstance<LocalizationService>();
            UnityEditor.AssetDatabase.CreateAsset(instance, "Assets" + fileName);

            instance.currentKit = ScriptableObject.CreateInstance<LocalizationKit>();
            UnityEditor.AssetDatabase.CreateAsset(instance.currentKit, "Assets" + path + "LocKit_fa.asset");

            UnityEditor.AssetDatabase.SaveAssets();
        }

        public static string Get(int id)
        {
            if (Instance.currentKit)
                return Instance.currentKit.Get(id);
            else
                return id.ToString();
        }

        public static int UpdateString(int id, string text)
        {
            if (Instance.currentKit)
                return Instance.currentKit.UpdateString(id, text);
            else
                return id;
        }
#endif
    }
}