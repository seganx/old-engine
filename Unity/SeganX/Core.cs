using System.IO;
using UnityEngine;

namespace SeganX
{
    public class Core : ScriptableObject
    {
        public class Data
        {
            public string baseDeviceId;
            public string deviceId;
            public string saltHash;
            public byte[] cryptoKey;

            public Data(string key, string salt, bool hashSalt)
            {
                baseDeviceId = SystemInfo.deviceUniqueIdentifier;
                deviceId = ComputeMD5(baseDeviceId, salt);
                saltHash = hashSalt ? ComputeMD5(salt, salt) : salt;
                cryptoKey = System.Text.Encoding.ASCII.GetBytes(key);
            }
        }

        public string cryptokey = "";
        public string salt = "";
        public bool hashSalt = false;
        public Shader[] shaders;

        public Data data = null;

        private void Awake()
        {
            data = new Data(cryptokey, salt, hashSalt);
#if UNITY_EDITOR
#else
            cryptokey = "";
            salt = "";
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