using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System.Collections;

namespace SeganX
{
    public class Core : MonoBehaviour
    {
        public string cryptokey = "";
        public string salt = "";
        public Text screenlog = null;

        public Shader[] shaders;


        // Use this for initialization
        IEnumerator Start()
        {
            Salt = salt;
            CryptoKey = System.Text.Encoding.ASCII.GetBytes(cryptokey);

            BaseDeviceId = PlayerPrefs.GetString("Core.BaseDeviceId", "");
            if (BaseDeviceId.Length < 1)
            {
#if UNITY_EDITOR || UNITY_ANDROID || UNITY_IOS
                //  Collect advertising ID
                screenlog.text = "Version " + Application.version + "\n";
                screenlog.text += "Getting advertising id...\n";
                Application.RequestAdvertisingIdentifierAsync((adId, trackingEnabled, error) =>
                {
                    screenlog.text += "AdvertisingId: " + adId + "\nTrackingEnabled: " + trackingEnabled + "\n";
                    BaseDeviceId = string.IsNullOrEmpty(error) ? adId : System.Guid.NewGuid().ToString();
                    PlayerPrefs.SetString("Core.BaseDeviceId", BaseDeviceId);
                });
#else
            AdvertisingId = SystemInfo.deviceUniqueIdentifier;
#endif
            }

            //  wait for response of the request
#if !UNITY_EDITOR
        var startTime = System.DateTime.Now;
        while (BaseDeviceId.Length < 1 && (System.DateTime.Now - startTime).TotalSeconds < 5)
            yield return new WaitForEndOfFrame();
#endif

            //  validate advertising Id
            if (BaseDeviceId.Length < 1)
            {
                screenlog.text += "Failed to getting Advertising Id\nGetting MAC address...\n";
                //  Collect MAC address
#if UNITY_ANDROID
                try
                {
                    var activity = new AndroidJavaClass("com.unity3d.player.UnityPlayer").GetStatic<AndroidJavaObject>("currentActivity");
                    var wifiManager = activity.Call<AndroidJavaObject>("getSystemService", "wifi");
                    var macaddress = wifiManager.Call<AndroidJavaObject>("getConnectionInfo").Call<string>("getMacAddress");
                    if (macaddress.IndexOf("02:00:00", 0) < 0 && macaddress.IndexOf("00:00:00", 0) < 0)
                    {
                        BaseDeviceId = macaddress;
                        PlayerPrefs.SetString("Core.BaseDeviceId", BaseDeviceId);
                    }
                    screenlog.text += "MAC address: " + macaddress;
                }
                catch (System.Exception ex)
                {
                    screenlog.text += "Failed to getting MAC address!\nError: " + ex + "\n";
                }
#elif UNITY_IOS
#else
#endif
                yield return new WaitForSeconds(1);
            }

            //  validate base device id
            if (BaseDeviceId.Length < 1)
            {
                IsFakeDeviceId = true;
                BaseDeviceId = System.Guid.NewGuid().ToString();
                PlayerPrefs.SetString("Core.BaseDeviceId", BaseDeviceId);
            }
            else IsFakeDeviceId = false;

            //  verify and load unique device id
            DeviceId = PlayerPrefs.GetString("Core.DeviceId", "");
            if (DeviceId.Length < 1)
            {
                DeviceId = ComputeMD5(BaseDeviceId, salt);
                PlayerPrefs.SetString("Core.DeviceId", DeviceId);
            }

            SceneManager.LoadScene(1, LoadSceneMode.Single);
        }


        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        public static string BaseDeviceId { private set; get; }
        public static string DeviceId { private set; get; }
        public static string Salt { private set; get; }
        public static bool IsFakeDeviceId { private set; get; }
        public static byte[] CryptoKey { private set; get; }

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


    }
}