using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Threading;
using System.Text;
using System.Net;
using System.Net.Sockets;

[Serializable()]
public struct NetHeader
{
    public ushort netId;         //!	net id helps to avoid servers conflicts
    public ushort number;        //! message number
    public ushort option;        //! message option contains message flag
    public ushort checksum;      //! checksum of the content data
};

//! describe a package content
public class NetPackage
{
    private int _headerSize;
    public NetPackage(int headerSize, int bufferSize)
    {
        size = headerSize;						//! size of all data contain header and user data
        _headerSize = headerSize;						//! size of all data contain header and user data
        data = new byte[bufferSize];		//! it also contains header
        header = new NetHeader();      //! no allocation needed. It just points to data
    }

    public int size;						//! size of all data contain header and user data
    public byte[] data;		//! it also contains header
    public NetHeader header;      //! no allocation needed. It just points to data
    //public void UpdateData()
    //{
    //    Buffer.BlockCopy(Utils.ObjectToByteArray(header), 0, data, 0, _headerSize);
    //}
}

public class NetBase : MonoBehaviour
{
    public int localPort = 31000;
    public ushort NET_ID = 31;

    [HideInInspector]
    public int NET_BROADCAST = 0;
    [HideInInspector]
    public int NET_BUFF_SIZE = 1024;	//! maximum size of a package in bytes
    [HideInInspector]
    public ushort NET_OPTN_SAFESEND = 0x01;	//! used in message header and indicates that the message should be resend till other sides confirms
    [HideInInspector]
    public ushort NET_OPTN_CONFIRMED = 0x02;	//! used in message header and indicates that the message is confirmation by the other sides
    [HideInInspector]
    public int NET_HEADER_SIZE = 8;

    public MySocket socket;

    [HideInInspector]
    public List<Connection> connections = new List<Connection>();
    public Connection CreateConnection(IPEndPoint address)
    {
        GameObject go = new GameObject("Conn" + connections.Count.ToString());
        go.transform.SetParent(transform);

        Connection conn = go.AddComponent<Connection>();
        conn.Open(this, address);
        connections.Add(conn);

        return conn;
    }

    public byte[] NetHeaderToByteArray(NetHeader header)
    {
        byte[] buffer = new byte[NET_HEADER_SIZE];
        Buffer.BlockCopy(BitConverter.GetBytes(header.netId), 0, buffer, 0, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.number), 0, buffer, 2, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.option), 0, buffer, 4, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.checksum), 0, buffer, 6, 2);
        return buffer;
    }

    public NetHeader ByteArrayToNetHeader(byte[] data)
    {
        NetHeader nh = new NetHeader();
        nh.netId = BitConverter.ToUInt16(data, 0);
        nh.number = BitConverter.ToUInt16(data, 2);
        nh.option = BitConverter.ToUInt16(data, 4);
        nh.checksum = BitConverter.ToUInt16(data, 6);
        return nh;
    }

    public ushort ComputeChecksum(byte[] buffer)
    {
        ushort res = NET_ID;
        for (int i = 0; i < buffer.Length; ++i)
            res += (ushort)((res + buffer[i]) * NET_ID);
        return res;
    }

    public bool VerifyPackage(byte[] buffer, int size, ulong lastNumber)
    {
        //	validate message size
        if (size < NET_HEADER_SIZE || size > NET_BUFF_SIZE)
            return false;

        NetHeader nh = ByteArrayToNetHeader(buffer);

        //	validate net id
        if (nh.netId != NET_ID)
            return false;

        //	validate if message is duplicated
        //if ( nh.number == lastNumber && set_hasnt( nh.option, NET_OPTN_SAFESEND ) )
        //    return false;

        //	validate data checksum
        if (size > NET_HEADER_SIZE)
        {
            if (nh.checksum != ComputeChecksum(buffer))
                return false;
        }

        // new we can suppose that the package is valid
        return true;
    }

    void Start()
    {
        socket = new MySocket();
        socket.Open(localPort);
    }

    void Update()
    {
        PeekReceivedMessages();
    }

    void OnApplicationQuit()
    {
        //rcvThread.Abort();
    }

    void PeekReceivedMessages()
    {
        byte[] buffer = new byte[NET_BUFF_SIZE];

        int receivedBytes = 0;
        do
        {
            IPEndPoint address = new IPEndPoint(IPAddress.Any, localPort);

            //	peek the package
            receivedBytes = socket.Receive(ref buffer, ref address);
            if (receivedBytes < 1) continue;

            //	check if we have a connection for the received address
            if (connections.Exists(x => (x.dest.Address.ToString() == address.Address.ToString())))
            {
                Connection conn = connections.Find(x => x.dest.Address.ToString() == address.Address.ToString());

                //	verify that the package is valid
                if (VerifyPackage(buffer, receivedBytes, conn.recvNumber))
                {
                    //	append the package to the connection buffer
                    conn.AppendReceivedMessage(buffer, receivedBytes);
                }
            }

            Thread.Sleep(100);
        }
        while (receivedBytes > 0);
    }
}

