using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Test : MonoBehaviour
{
    public string serverAddress;

    [Header("Authenticator")]
    public string authenUri = "/authen";
    public string accessUri = "/access";

    [Header("Authentication params:")]
    public string deviceId;
    public string google;
    public string facebook;
    public string type;
    public string username;
    public string password;
    

    [System.Serializable]
    public class JsonAuthenRec
    {
        public string user_data;
        public string public_key;
        public string auth_code;
    }


    IEnumerator AuthenSendRequest()
    {
        while (true)
        {
            byte[] secretKey = AuthenService.SecretKey(32);
            byte[] publicKey = AuthenService.PublicKey(secretKey, 7, 23);
            string msg = "{\"user_data\":63,\"public_key\":" + System.Text.ASCIIEncoding.ASCII.GetString(publicKey) + "}";

            Debug.Log("sending: " + msg);
            WWW ws = new WWW(serverAddress + authenUri, System.Text.Encoding.ASCII.GetBytes(msg));

            yield return ws;
            Debug.Log("received: " + ws.text);

            JsonAuthenRec jsk = JsonUtility.FromJson<JsonAuthenRec>(ws.text);
            if (jsk.user_data == "63")
            {
                byte[] rcvd_key = System.Text.Encoding.ASCII.GetBytes(jsk.public_key);
                byte[] final_key = AuthenService.FinalKey(secretKey, rcvd_key, 23);
                Debug.Log("Key: " + System.Text.ASCIIEncoding.ASCII.GetString(final_key));

                msg = "{\"user_data\":63"+
                    ",\"device\":" + deviceId +
                    ",\"google\":" + google +
                    ",\"facebook\":" + facebook +
                    ",\"type\":" + type +
                    ",\"user\":" + username + 
                    ",\"pass\":" + password + 
                    "}";
                Debug.Log("sending: " + msg);
                byte[] authencode = System.Text.Encoding.ASCII.GetBytes(jsk.auth_code);
                byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), final_key);
                byte[] finalmsg = new byte[authencode.Length + endata.Length];
                System.Buffer.BlockCopy(authencode, 0, finalmsg, 0, authencode.Length);
                System.Buffer.BlockCopy(endata, 0, finalmsg, authencode.Length, endata.Length);
                ws = new WWW(serverAddress + accessUri, finalmsg);
                yield return ws;
                Debug.Log("received: " + ws.text);
            }
        }
    }

    void OnGUI()
    {
        int y = 10, w = 120;
        if (GUI.Button(new Rect(10, y, w, 30), "send request"))
            StartCoroutine(AuthenSendRequest());
    }
}
