using UnityEngine;
using System.Collections;
using System.Net;
using System.Text;

public class NetClient : MonoBehaviour
{
    public NetSocket socket = null;

    //  address of the server, 127.0.0.1 is for your own computer
    public string serverIP = "127.0.0.1";

    //  port for the server, make sure to unblock this in your router firewall if you want to allow external connections
    public int serverPort = 31000;

    //  connection handler
    Connection con = new Connection();

    //  end point address
    private IPEndPoint destination;

    // Use this for initialization
    void Start()
    {
        // preparing destination address
        destination = new IPEndPoint(IPAddress.Parse(serverIP), serverPort);
        con.Open(destination);
    }

    // Update is called once per frame
    void Update()
    {
        PeekReceivedMessages();

        con.Update(socket, Time.deltaTime);
    }

    public void SendTestDataToServer(string nickname)
    {
        string tmp = UnityEngine.SystemInfo.deviceUniqueIdentifier + '#' + nickname;
        byte[] msg = Encoding.UTF8.GetBytes(tmp);
        con.Send(msg, NetBase.NET_OPTN_SAFESEND);
    }

    void OnGUI()
    {
        if (GUI.Button(new Rect(0, 0, 100, 50), "Send"))
            SendTestDataToServer("sajad beigjani");
    }

    void PeekReceivedMessages()
    {
        byte[] buffer = new byte[NetBase.NET_BUFF_SIZE];

        int receivedBytes = 0;
        do
        {
            //	peek the package
            receivedBytes = socket.Received(buffer, destination.Address);
            if (receivedBytes < 1) continue;

            //	verify that the package is valid
            if (NetBase.VerifyPackage(buffer, receivedBytes, con.recvNumber))
            {
                //	append the package to the connection buffer
                con.AppendReceivedMessage(socket, buffer, receivedBytes);
            }
        }
        while (receivedBytes > 0);
    }
}
