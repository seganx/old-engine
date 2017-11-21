using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace SeganX
{
    [CustomEditor(typeof(Core))]
    public class CoreEditor : UnityEditor.Editor
    {
        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            var core = target as Core;
            if (GUILayout.Button("Encrypt File"))
            {
                var path = EditorUtility.OpenFilePanel("Encrypt File And Save", "", "");
                CryptoService.SaveEncryptFile(path, core.cryptokey.GetBytes(), core.salt);
            }
            if (GUILayout.Button("Decrypt File"))
            {
                var path = EditorUtility.OpenFilePanel("Decrypt File And Save", "", "");
                CryptoService.SaveDecryptFile(path, core.cryptokey.GetBytes(), core.salt);
            }
        }
    }
}
