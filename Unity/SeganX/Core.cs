using System.IO;
using UnityEngine;

namespace SeganX
{
    public class Core : ScriptableObject
    {
        [System.Serializable]
        public class SecurityOptions
        {
            public string cryptokey = "";
            public string salt = "";
            public bool hashSalt = true;
        }

        public class Data
        {
            public string baseDeviceId;
            public string deviceId;
            public string saltHash;
            public byte[] cryptoKey;
            public int valueKey = 77777;

            public Data(string key, string salt, bool hashSalt)
            {
                baseDeviceId = SystemInfo.deviceUniqueIdentifier;
                deviceId = ComputeMD5(baseDeviceId, salt);
                saltHash = hashSalt ? ComputeMD5(salt, salt) : salt;
                cryptoKey = System.Text.Encoding.ASCII.GetBytes(key);

                foreach (var s in key)
                    valueKey += valueKey + s;
            }
        }

#if UNITY_EDITOR
        [System.Serializable]
        public class AssetBundleBuildOptions
        {
            [System.Serializable]
            public class BuildOptions
            {
                public enum TextureResize : int { FullSize = 1, HalfSize = 2, QuarterSize = 4 }
                public string folder = "android";
                public string suffix = "";
                public UnityEditor.BuildTarget platform = UnityEditor.BuildTarget.Android;
                public TextureResize textureSize = TextureResize.FullSize;
                public bool encrypt = true;
                public bool active = true;
            }

            public string bundlesPath = "Assets/Editor/Bundles";
            public string outputPath = "AssetBundles";
            public UnityEditor.BuildAssetBundleOptions buildOptions = UnityEditor.BuildAssetBundleOptions.None;
            public BuildOptions[] builds = null;
        }
#endif

        public SecurityOptions securityOptions;
#if UNITY_EDITOR
        public AssetBundleBuildOptions assetBundleBuildOptions;
#endif

        [Header("Deprecated: Use Security Options")]
        [System.Obsolete("Use SecurityOptions.cryptokey")]
        public string cryptokey = "";
        [System.Obsolete("Use SecurityOptions.salt")]
        public string salt = "";
        [System.Obsolete("Use SecurityOptions.hashSalt")]
        public bool hashSalt = false;

        public Data data = null;

        private void Awake()
        {
            data = new Data(securityOptions.cryptokey, securityOptions.salt, securityOptions.hashSalt);
#if UNITY_EDITOR
#else
            cryptokey = "";
            salt = "";
            securityOptions.cryptokey = "";
            securityOptions.salt = "";
#endif
        }

        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        private static Core instance = null;

        public static string BaseDeviceId { get { return Instance.data.baseDeviceId; } }
        public static string DeviceId { get { return Instance.data.deviceId; } }
        public static string Salt { get { return Instance.data.saltHash; } }
        public static byte[] CryptoKey { get { return Instance.data.cryptoKey; } }
        public static int ValueKey { get { return Instance.data.valueKey; } }

        public static Core Instance
        {
            get
            {
#if UNITY_EDITOR
                CheckService();
#endif
                if (instance == null) instance = Resources.Load<Core>("SeganX");
                if (instance != null && instance.data == null) instance.Awake();
                return instance;
            }
        }

        public static string ComputeMD5(string str, string salt)
        {
            var md5 = System.Security.Cryptography.MD5.Create();
            byte[] inputBytes = System.Text.Encoding.ASCII.GetBytes(str + salt);
            byte[] hashBytes = md5.ComputeHash(inputBytes);

            var res = new System.Text.StringBuilder();
            for (int i = 0; i < hashBytes.Length; i++)
                res.Append(hashBytes[i].ToString("X2"));

            return res.ToString();
        }


#if UNITY_EDITOR
        public static void CheckService()
        {
            var path = "/Resources/";
            var fileName = path + "SeganX.asset";
            if (File.Exists(Application.dataPath + fileName)) return;

            var ioPath = Application.dataPath + path;
            if (!Directory.Exists(ioPath)) Directory.CreateDirectory(ioPath);

            instance = CreateInstance<Core>();
            UnityEditor.AssetDatabase.CreateAsset(instance, "Assets" + fileName);

            UnityEditor.AssetDatabase.SaveAssets();
        }
#endif
    }
}