using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Test : MonoBehaviour
{
    public string serverAddress;

    [Header("Authenticator")]
    public string uriCode = "/authen/code";
    public string uriDevice = "/authen/device";
    public string uriUserPassLogin = "/authen/userpass/login";
    public string uriUserPassRegister = "/authen/userpass/register";

    [Header("Authentication params:")]
    public string deviceId;
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
    public JsonAuthenRec authenCode;
    public byte[] finalKey;

    IEnumerator RequestAuthenCode()
    {
        byte[] secretKey = AuthenService.SecretKey(32);
        byte[] publicKey = AuthenService.PublicKey(secretKey, 7, 23);
        string msg = "{\"user_data\":63,\"public_key\":" + System.Text.ASCIIEncoding.ASCII.GetString(publicKey) + "}";

        Debug.Log("sending: " + msg);
        WWW ws = new WWW(serverAddress + uriCode, System.Text.Encoding.ASCII.GetBytes(msg));

        yield return ws;
        Debug.Log("received: " + ws.text);

        authenCode = JsonUtility.FromJson<JsonAuthenRec>(ws.text);
        if (authenCode.user_data == "63")
        {
            byte[] rcvd_key = System.Text.Encoding.ASCII.GetBytes(authenCode.public_key);
            finalKey = AuthenService.FinalKey(secretKey, rcvd_key, 23);
            Debug.Log("Key: " + System.Text.ASCIIEncoding.ASCII.GetString(finalKey));
        }
    }

    IEnumerator LoginWithDevice()
    {
        if (authenCode.user_data == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"user_data\":63" +
                ",\"device\":" + deviceId +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(authenCode.auth_code);
            byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), finalKey);
            byte[] finalmsg = new byte[authencode.Length + endata.Length];
            System.Buffer.BlockCopy(authencode, 0, finalmsg, 0, authencode.Length);
            System.Buffer.BlockCopy(endata, 0, finalmsg, authencode.Length, endata.Length);

            var ws = new WWW(serverAddress + uriDevice, finalmsg);
            yield return ws;
            Debug.Log("received: " + ws.text);
        }
    }

    IEnumerator RegisterUserPass()
    {
        if (authenCode.user_data == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"user_data\":63" +
                ",\"device\":" + deviceId +
                ",\"user\":" + username +
                ",\"pass\":" + password +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(authenCode.auth_code);
            byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), finalKey);
            byte[] finalmsg = new byte[authencode.Length + endata.Length];
            System.Buffer.BlockCopy(authencode, 0, finalmsg, 0, authencode.Length);
            System.Buffer.BlockCopy(endata, 0, finalmsg, authencode.Length, endata.Length);

            var ws = new WWW(serverAddress + uriUserPassRegister, finalmsg);
            yield return ws;
            Debug.Log("received: " + ws.text);
        }
    }

    IEnumerator LoginWithUserPass()
    {
        if (authenCode.user_data == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"user_data\":63" +
                ",\"device\":" + deviceId +
                ",\"user\":" + username +
                ",\"pass\":" + password +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(authenCode.auth_code);
            byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), finalKey);
            byte[] finalmsg = new byte[authencode.Length + endata.Length];
            System.Buffer.BlockCopy(authencode, 0, finalmsg, 0, authencode.Length);
            System.Buffer.BlockCopy(endata, 0, finalmsg, authencode.Length, endata.Length);

            var ws = new WWW(serverAddress + uriUserPassLogin, finalmsg);
            yield return ws;
            Debug.Log("received: " + ws.text);
        }
    }


    void OnGUI()
    {
        int y = 10, w = 120;
        if (GUI.Button(new Rect(10, y += 40, w, 30), "Request Code"))
            StartCoroutine(RequestAuthenCode());

        if (GUI.Button(new Rect(10, y += 40, w, 30), "Login Device"))
            StartCoroutine(LoginWithDevice());

        if (GUI.Button(new Rect(10, y += 40, w, 30), "Register UserPass"))
            StartCoroutine(RegisterUserPass());

        if (GUI.Button(new Rect(10, y += 40, w, 30), "Login UserPass"))
            StartCoroutine(LoginWithUserPass());
    }
}
