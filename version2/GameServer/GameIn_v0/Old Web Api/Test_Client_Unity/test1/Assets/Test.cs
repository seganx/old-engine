using UnityEngine;
using System.IO;
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
    public string gameKey;
    public string deviceId;
    public string type;
    public string username;
    public string password;


    public byte[] finalKey;

    public GameObject cube = null;

    private static string log;

    [System.Serializable]
    public class GameinHeader
    {
        public string userdata;
        public string error;
        public string access;
    }
    public GameinHeader gameinHeader;


    IEnumerator RequestAuthenCode()
    {
        //  request authentication ticket
        byte[] secretKey = AuthenService.SecretKey(32);
        byte[] publicKey = AuthenService.PublicKey(secretKey, 7, 23);

        var ws = PostWWW(serverAddress + uriCode, "63", "", publicKey);
        yield return ws;
        Debug.Log("received: " + ws.text);

        gameinHeader = GetHeader(ws);
        if (gameinHeader.userdata == "63")
        {
            byte[] rcvd_key = System.Text.Encoding.ASCII.GetBytes(ws.text);
            finalKey = AuthenService.FinalKey(secretKey, rcvd_key, 23);
            Debug.Log("Key: " + System.Text.ASCIIEncoding.ASCII.GetString(finalKey));
        }
    }

    IEnumerator LoginWithDevice()
    {
        if (gameinHeader.userdata == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"game\":" + gameKey +
                ",\"device\":" + deviceId +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(gameinHeader.access);
            byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), finalKey);

            var ws = PostWWW(serverAddress + uriDevice, gameinHeader.userdata, gameinHeader.access, endata);
            yield return ws;
            gameinHeader = GetHeader(ws);
            Debug.Log("received: " + ws.text);
        }
    }

    IEnumerator RegisterUserPass()
    {
        if (gameinHeader.userdata == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"user_data\":63" +
                ",\"device\":" + deviceId +
                ",\"user\":" + username +
                ",\"pass\":" + password +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(gameinHeader.access);
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
        if (gameinHeader.userdata == "63")
        {
            var msg = "{" +
                "\"ver\":1" +
                ",\"user_data\":63" +
                ",\"device\":" + deviceId +
                ",\"user\":" + username +
                ",\"pass\":" + password +
                "}";
            Debug.Log("sending: " + msg);

            byte[] authencode = System.Text.Encoding.ASCII.GetBytes(gameinHeader.access);
            byte[] endata = AuthenService.Encrypt(System.Text.Encoding.ASCII.GetBytes(msg), finalKey);
            byte[] finalmsg = new byte[authencode.Length + endata.Length];
            System.Buffer.BlockCopy(authencode, 0, finalmsg, 0, authencode.Length);
            System.Buffer.BlockCopy(endata, 0, finalmsg, authencode.Length, endata.Length);

            var ws = new WWW(serverAddress + uriUserPassLogin, finalmsg);
            yield return ws;
            Debug.Log("received: " + ws.text);
        }
    }

    public static GameinHeader GetHeader(WWW ws)
    {
        var res = new GameinHeader();
        ws.responseHeaders.TryGetValue("GAMEIN_USERDATA", out res.userdata);
        ws.responseHeaders.TryGetValue("GAMEIN_ERROR", out res.error);
        ws.responseHeaders.TryGetValue("GAMEIN_ACCESS", out res.access);
        return res;
    }

    public static WWW PostWWW(string uri, string userdata, string access, byte[] data)
    {
        Dictionary<string, string> headers = new Dictionary<string, string>();
        headers.Add("GAMEIN_USERDATA", userdata);
        headers.Add("GAMEIN_ACCESS", access);
        return new WWW(uri, data, headers);
    }



    IEnumerator DownloadTexture()
    {
        var ws = new WWW("http://locator.8khan.ir/G315.tex");
        log += "Downloading -> ";
        yield return ws;
        log += "Downloaded: " + ws.size + "\n";
        SaveData("test", ws.bytes);
    }

    IEnumerator DisplayTexture()
    {
        var tex = new Texture2D(256, 256, TextureFormat.DXT1, false);
        tex.LoadRawTextureData(LoadData("test"));
        tex.Apply();
        cube.GetComponent<Renderer>().material.mainTexture = tex;        
        yield break;
    }


    void OnGUI()
    {
        GUI.Box(new Rect(0, 0, Screen.width, 100), log);

        int y = 80, w = 180, h = 20, ha = 30;

        if (GUI.Button(new Rect(10, y += ha, w, h), "Download Texture"))
            StartCoroutine(DownloadTexture());

        if (GUI.Button(new Rect(10, y += ha, w, h), "Display Texture"))
            StartCoroutine(DisplayTexture());

        if (GUI.Button(new Rect(10, y += ha, w, h), "Request Code"))
            StartCoroutine(RequestAuthenCode());

        if (GUI.Button(new Rect(10, y += ha, w, h), "Login Device"))
            StartCoroutine(LoginWithDevice());

        if (GUI.Button(new Rect(10, y += ha, w, h), "Register UserPass"))
            StartCoroutine(RegisterUserPass());

        if (GUI.Button(new Rect(10, y += ha, w, h), "Login UserPass"))
            StartCoroutine(LoginWithUserPass());
    }


    public static void SaveData(string path, byte[] data)
    {
        path = Application.temporaryCachePath + "/" + path;
        log += "Saving to " + path + "\n";
        var dir = Path.GetDirectoryName(path);
        if (Directory.Exists(dir) == false)
            Directory.CreateDirectory(dir);
        try
        {
            File.WriteAllBytes(path, data);
            log += "Saved!\n";
        }
        catch { }
    }

    public static byte[] LoadData(string path)
    {
        byte[] res = null;
        path = Application.temporaryCachePath + "/" + path;
        log += "Loading from " + path + "\n";
        if (File.Exists(path))
        {
            try
            {
                res = File.ReadAllBytes(path);
                log += "File read: " + (res == null ? "0" : res.Length.ToString()) + " Bytes\n";
            }
            catch (System.Exception e)
            {
                log += "Can't read file: " + e.Message;
            }
        }
        else log += "File not exist!!\n";
        return res;
    }
}
