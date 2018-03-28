﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [CreateAssetMenu(menuName = "Game/LocalizationKit")]
    public class LocalizationKit : ScriptableObject
    {
        [System.Serializable]
        public class LocalKitData
        {
            [System.Serializable]
            public class LocalStrings
            {
                public int i = 0;
                public string s = "";
                public override string ToString() { return i + ":" + s; }
            }

            public string language = "fa persian farsi";
            public List<LocalStrings> strings = new List<LocalStrings>();
        }


        public LocalKitData kit = new LocalKitData();

        public string Get(int id)
        {
            var obj = kit.strings.Find(x => x.i == id);
            return (obj == null) ? id.ToString() : obj.s;
        }

#if UNITY_EDITOR
        private void OnEnable()
        {
            LocalizationService.CheckService();
            LocalizationService.kits.Remove(this);
            LocalizationService.kits.Add(this);
        }

        private void OnDisable()
        {
            LocalizationService.kits.Remove(this);
        }

        public int AddString(string text)
        {
            var res = new LocalKitData.LocalStrings() { i = LocalizationService.Instance.baseId++, s = text };
            if (text == null) res.s = res.i.ToString();
            kit.strings.Add(res);
            return res.i;
        }

        public int UpdateString(int index, string text)
        {
            var res = kit.strings.Find(x => x.i == index);
            if (res == null)
            {
                res = new LocalKitData.LocalStrings() { i = LocalizationService.Instance.baseId++, s = text };
                kit.strings.Add(res);
            }
            else res.s = text;
            return res.i;
        }
#endif
    }
}
