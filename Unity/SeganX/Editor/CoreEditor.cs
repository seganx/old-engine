using System.IO;
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
                if (path.Length > 3)
                {
                    var src = File.ReadAllBytes(path);
                    var data = CryptoService.EncryptWithMac(src, core.cryptokey.GetBytes(), core.salt);
#if UNITY_STANDALONE_WIN
                    File.WriteAllBytes(path + ".seganx.win", data);
#else
                    File.WriteAllBytes(path + ".seganx", data);
#endif
                }
            }
            if (GUILayout.Button("Decrypt File"))
            {
                var path = EditorUtility.OpenFilePanel("Decrypt File And Save", "", "");
                if (path.Length > 3)
                {
                    var src = File.ReadAllBytes(path);
                    var data = CryptoService.DecryptWithMac(src, core.cryptokey.GetBytes(), core.salt);
#if UNITY_STANDALONE_WIN
                    File.WriteAllBytes(path.Replace(".seganx.win", ""), data);
#else
                    File.WriteAllBytes(path.Replace(".seganx", ""), data);
#endif
                }
            }
        }
    }
}
