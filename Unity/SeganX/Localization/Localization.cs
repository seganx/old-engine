using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [System.Serializable]
    public class LocalizationKit
    {
        public enum Language { fa = 0, en = 1 }

        [System.Serializable]
        public class LocalStrings
        {
            public int i = 0;
            public string s = "";
            public override string ToString() { return i + ":" + s; }
        }

        public int baseId = 911000;
        public Language language = Localization.CurrentLanguage;
        public List<LocalStrings> strings = new List<LocalStrings>();
    }

    [CreateAssetMenu(menuName = "Game/Localization")]
    public class Localization : ScriptableObject
    {
        public LocalizationKit kit = new LocalizationKit();

        public string Get(int id)
        {
            var obj = kit.strings.Find(x => x.i == id);
            return (obj == null) ? "" : obj.s;
        }

#if UNITY_EDITOR
        private string BaseFilePath { get { return UnityEditor.AssetDatabase.GetAssetPath(this).ExcludeFileExtention() + "_"; } }

        private void OnEnable()
        {
            all.Remove(this);
            all.Add(this);
        }

        private void OnDisable()
        {
            all.Remove(this);
            Save();
        }

        public int UpdateString(int index, string text)
        {
            var res = kit.strings.Find(x => x.i == index);
            if (res == null)
            {
                res = new LocalizationKit.LocalStrings() { i = kit.baseId++, s = text };
                kit.strings.Add(res);
            }
            else res.s = text;
            return res.i;
        }

        public void Save()
        {
            var json = JsonUtility.ToJson(kit, true);
            System.IO.File.WriteAllText(BaseFilePath + kit.language + ".json", json, System.Text.Encoding.UTF8);
        }

        public void Load()
        {
            var path = BaseFilePath + CurrentLanguage + ".json";
            if (System.IO.File.Exists(path))
            {
                var json = System.IO.File.ReadAllText(path, System.Text.Encoding.UTF8);
                kit = JsonUtility.FromJson<LocalizationKit>(json);
            }
            else
            {
                kit = new LocalizationKit();
                kit.language = CurrentLanguage;
            }
        }

        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        public static LocalizationKit.Language CurrentLanguage = LocalizationKit.Language.fa;
        public static List<Localization> all = new List<Localization>();
#endif
    }
}
