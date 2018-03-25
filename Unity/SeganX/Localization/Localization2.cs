using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [System.Serializable]
    public class LocalizationKit2
    {
        [System.Serializable]
        public class LocalStrings
        {
            public int i = 0;
            public string s = "";
            public override string ToString() { return i + ":" + s; }
        }

        public int baseId = 911000;
        public string language = "fa persian farsi";
        public List<LocalStrings> strings = new List<LocalStrings>();
    }

    [CreateAssetMenu(menuName = "Game/Localization2")]
    public class Localization2 : ScriptableObject
    {
        public string currnetLanguage = "fa";
        public string kitsPath = "Resources/LocalKits/";
        public List<string> kits = new List<string>();

        public void OnValidation()
        {
            Debug.Log(currnetLanguage);
        }

        #region standard functions
        private void Awake()
        {
            var validPath = Application.dataPath + "/" + kitsPath;
            if (System.IO.Directory.Exists(validPath) == false)
                System.IO.Directory.CreateDirectory(validPath);
        }
        #endregion
    }
}
