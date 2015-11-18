using UnityEngine;
using System.Collections;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

//! basic UPD socket class
public class MySocket
{
    UdpClient udpClient = null;

    [HideInInspector]
    int localPort = 0;
    
    //public bool isOpenned { get { return udpClient != null; } }

    public MySocket()
    {
        if (udpClient != null)
            Close();
    }

    public bool Open(int port)
    {
        try
        {
            if (udpClient != null)
                Close();

            // create socket
            udpClient = new UdpClient(port);
            localPort = port;

            // Set non block
            udpClient.Client.Blocking = false;

            // Begin receive
            //BeginReceive();

            return true;
        }
        catch
        {
            Debug.LogError("Open Socket Failed");
            return false;
        }
    }

    public void Close()
    {
        if (udpClient != null)
            udpClient.Close();
    }

    public bool Send(IPEndPoint destination, byte[] buffer, int size)
    {
        if (udpClient == null || buffer == null || buffer.Length < size || size < 1)
        {
            Debug.LogError("Send failed!");
            return false;
        }

        try
        {
            //udpClient.Connect(destination.Address.ToString(), destination.Port);

            // Sends a message to the host to which you have connected.
            int sentBytes = udpClient.Send(buffer, size, destination);

            return sentBytes == size;
        }
        catch
        {
            return false;
        }
    }

    public int Receive(ref byte[] receiveBytes, ref IPEndPoint anyIP)
    {
        if (udpClient.Client.Available < 1)
            return 0;

        Debug.Log("Available: " + udpClient.Client.Available.ToString());

        try
        {
            receiveBytes = udpClient.Receive(ref anyIP);
        }
        catch
        {
            return 0;
        }

        if (receiveBytes.Length == 0)
            return 0;

        string receiveString = Encoding.Unicode.GetString(receiveBytes);

        Debug.Log("Received Bytes Count: " + receiveBytes.Length.ToString());

        return receiveBytes.Length;
    }

    void OnApplicationQuit()
    {
        //if (receiving)
        //    StopCoroutine(ReceiveCoroutine());

        Close();
    }

    //public void ReceiveCallback(IAsyncResult ar)
    //{
    //    UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    //    IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    //    receiveBytes = u.EndReceive(ar, ref e);
    //    receiving = false;
    //    messageReceived = true;

    //    string receiveString = Encoding.ASCII.GetString(receiveBytes);

    //    Debug.Log(string.Format("Received: {0}", receiveString));
    //}

    //public void BeginReceive()
    //{
    //    if (udpClient == null || receiving)
    //    {
    //        // Debug.LogError
    //        return;
    //    }

    //    // Receive a message and write it to the console.
    //    IPEndPoint e = new IPEndPoint(IPAddress.Any, localPort);

    //    UdpState s = new UdpState();
    //    s.e = e;
    //    s.u = udpClient;

    //    Console.WriteLine("listening for messages");
    //    udpClient.BeginReceive(new AsyncCallback(ReceiveCallback), s);
    //    receiving = true;
    //    messageReceived = false;
    //}
}

public class UdpState
{
    public UdpClient u;
    public IPEndPoint e;
}