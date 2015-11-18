using UnityEngine;
using System.Collections;
using System;
using System.Net;
using System.Net.Sockets;


public class NetSocket : MonoBehaviour
{
    //  current client port. make sure to unblock this in your router firewall if you want to allow external connections
    public int port = 31000;

    //  variable for connection status
    private Socket socket = null;
    
    void Start()
    {
        //  create socket
        socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        socket.Blocking = false;
        socket.Bind(new IPEndPoint(IPAddress.Any, port));
    }

    void OnDestroy()
    {
        socket.Close();
    }

    public int Send(byte[] buffer, EndPoint destination)
    {
        return socket.SendTo(buffer, destination);
    }

    public int Received(byte[] buffer, IPAddress expectedAddress)
    {
        if (socket.Available > 7)
        {
            EndPoint tmp = new IPEndPoint(IPAddress.Any, 0);
            int res = 0;
            try
            {
                res = socket.ReceiveFrom(buffer, ref tmp);
            }
            catch { }            
            if (res > 0 && ((IPEndPoint)tmp).Address.Equals(expectedAddress))
                return res;
        }
        return 0;
    }

    public int Send(NetPacket packet, EndPoint destination)
    {
        if (packet.data != null && packet.data.Length > 0)
        {
            int size = NetBase.NET_HEADER_SIZE + packet.data.Length;
            byte[] buffer = new byte[size];

            // copy header information
            Buffer.BlockCopy(NetBase.NetHeaderToByteArray(packet.header), 0, buffer, 0, NetBase.NET_HEADER_SIZE);

            // copy data to the buffer
            Buffer.BlockCopy(packet.data, 0, buffer, NetBase.NET_HEADER_SIZE, packet.data.Length);

            return Send(buffer, destination);
        }
        else
        {
            int size = NetBase.NET_HEADER_SIZE;
            byte[] buffer = new byte[size];

            // copy header information
            Buffer.BlockCopy(NetBase.NetHeaderToByteArray(packet.header), 0, buffer, 0, NetBase.NET_HEADER_SIZE);

            return Send(buffer, destination);
        }
        
    }
}
