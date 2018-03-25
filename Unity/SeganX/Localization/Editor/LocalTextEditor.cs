using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace SeganX
{
    [CustomEditor(typeof(LocalText))]
    public class LocalTextEditor : Editor
    {
        public override void OnInspectorGUI()
        {
            //var loctext = target.As<LocalText>();
            //if (loctext.localization)
            //    loctext.currnetText = loctext.localization.Get(loctext.id);
            base.OnInspectorGUI();
        }
    }
}
