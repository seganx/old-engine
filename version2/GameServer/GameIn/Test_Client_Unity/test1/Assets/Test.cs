using UnityEngine;
using System.Collections;

public class Test : MonoBehaviour
{
    public string serverAddress;

    [Header("Authenticator")]
    public string authenUri = "/authen";

    [Header("CryptoService")]
    public string text;
    public uint key = 1363;

    [System.Serializable]
    public class JsonAuthenRec
    {
        public string user_data;
        public string id;
        public string key;
    }

    IEnumerator AuthenSendRequest()
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
            byte[] rcvd_key = System.Text.Encoding.ASCII.GetBytes(jsk.key);
            byte[] final_key = AuthenService.FinalKey(secretKey, rcvd_key, 23);

            uint key = CryptoService.Checksum(final_key);
            Debug.Log("Key: " + key);

            WWW d = new WWW(serverAddress + '/' + jsk.id);
            yield return d;
            Debug.Log(d.text);
        }
    }

    void OnGUI()
    {
        int y = 10, w = 120;
        if (GUI.Button(new Rect(10, y, w, 30), "send request"))
            StartCoroutine(AuthenSendRequest());

#if CR
                 WWWForm wf = new WWWForm();
            wf.AddField("user_data", "63");
            wf.AddBinaryData("public_key", publicKey);

        if (GUI.Button(new Rect(10, y += 35, w, 30), "crypto service"))
        {
            byte[] src = System.Text.Encoding.ASCII.GetBytes(text);
            uint chksum = checksum(src, key);
            Debug.Log("ch: " + chksum);

            byte[] dest = encrypt(src, key);

            string s = "";
            foreach (var b in dest)
                s += b.ToString() + " ";
            Debug.Log(s);

            dest = decrypt(dest, key);

            s = "";
            foreach (var b in dest)
                s += b.ToString() + " ";
            Debug.Log(s);
        }
#endif
    }
}
