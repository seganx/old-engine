using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;

public class Connection : MonoBehaviour
{
    public Connection()
    {
        isConnected = true;
        sentNumber = 1;
        recvNumber = 0;
        confirmNumber = 0;
        confirmChecksum = 0;
        delayTime = 0;
        delayTimeMax = 0.1f;
        retryTime = 0;
        retryTimeMax = 1;
        outTime = 0;
        outTimeMax = 60;
        //net = currentNetwork;
        //m_sendingPacks = new List<NetPackage>();		//!	list of packages for send
        //m_receivedPacks = new List<NetPackage>();	//!	list of received packages
    }

    public NetBase net;

    bool isConnected;		//! specified that the connection is established
    public IPEndPoint dest;		//! connection address
    public List<NetPackage> m_sendingPacks;// = new List<NetPackage>();		//!	list of packages for send
    public List<NetPackage> m_receivedPacks;// = new List<NetPackage>();	//!	list of received packages

    ushort sentNumber;		//! used in reliability system. hold the number of message sent
    public ushort recvNumber;		//! used in reliability system. hold the number of latest message has been received

    ushort confirmNumber;	//! indicates that connection is waiting for received confirmation
    ushort confirmChecksum;	//! indicates that connection is waiting for received confirmation

    double delayTime;		//!	delay time counter
    double delayTimeMax;		//! maximum delay time

    double retryTime;		//!	retry time counter
    double retryTimeMax;		//! maximum retry time

    double outTime;			//!	time out counter
    double outTimeMax;		//! maximum time out


    void Awake()
    {
        m_sendingPacks = new List<NetPackage>();		//!	list of packages for send
        m_receivedPacks = new List<NetPackage>();	//!	list of received packages
    }

    public void Open(NetBase network, IPEndPoint destination)
    {
        dest = destination;
        net = network;
    }

    void Close()
    {
        //	clear sending list
        m_sendingPacks.RemoveAll(x => true);

        //	clear received list
        m_receivedPacks.RemoveAll(x => true);

        //	clear address
        dest = null;
    }

    public void SetSpeed(int packPerSecond)
    {
        if (packPerSecond > 0)
            delayTimeMax = 1.0f / packPerSecond;
        else
            delayTimeMax = -1;
    }

    public void SetTimeOut(double timeOut)
    {
        outTimeMax = timeOut;
    }

    public bool Send(byte[] buffer, ushort option)
    {
        NetPackage np = new NetPackage(net.NET_HEADER_SIZE, net.NET_BUFF_SIZE);
        if ((buffer.Length + net.NET_HEADER_SIZE) > net.NET_BUFF_SIZE)
        {
            return false;
        }

        np.size = buffer.Length + net.NET_HEADER_SIZE;

        Buffer.BlockCopy(buffer, 0, np.data, net.NET_HEADER_SIZE, buffer.Length);

        np.header.netId = net.NET_ID;
        np.header.option = option;
        if (sentNumber < 1) sentNumber++;
        np.header.number = sentNumber++;
        byte[] tmpBuff = net.NetHeaderToByteArray(np.header);
        np.header.checksum = (ushort)net.ComputeChecksum(buffer);
        Buffer.BlockCopy(net.NetHeaderToByteArray(np.header), 0, np.data, 0, net.NET_HEADER_SIZE);

        m_sendingPacks.Add(np);

        return true;
    }

    public int Received(out byte[] destBuffer)
    {
        if (m_receivedPacks.Count > 0)
        {
            NetPackage np = m_receivedPacks[0];

            //	remove from the list
            m_receivedPacks.Remove(np);

            //	copy package data to the destination buffer
            int size = (int)np.size - net.NET_HEADER_SIZE;

            destBuffer = new byte[size];

            Buffer.BlockCopy(np.data, net.NET_HEADER_SIZE, destBuffer, 0, size);

            return size;
        }
        else
        {
            destBuffer = null;
            return 0;
        }
    }

    public void Update()
    {
        //	check connection time out to close the connection
        outTime += Time.deltaTime;
        //if (m_outTime > m_outTimeMax)
        //{
        //    // the connection is dropped
        //    m_outTime = 0;
        //    m_connected = false;
        //    //sx_print(L"Warning: Connection time out !\n");
        //}

        if (confirmNumber > 0)	//	we are waiting for received confirmation from the other side
        {
            //	check retry time out and resend the message
            retryTime += Time.deltaTime;
            if (retryTime > retryTimeMax)
            {
                retryTime = 0;

                if (m_sendingPacks.Count > 0)
                {
                    net.socket.Send(dest, m_sendingPacks[0].data, m_sendingPacks[0].size);
                }
                else
                {
                    confirmNumber = 0;  // confirmed
                    retryTime = 0;
                    outTime = 0;
                }
            }
        }
        else // just continue sending data
        {
            if (delayTimeMax > 0)
            {
                delayTime += Time.deltaTime;
                if (delayTime < delayTimeMax) return;
                delayTime = 0;
            }

            while (m_sendingPacks.Count > 0 && confirmNumber == 0)	// is there any message to send ?
            {
                NetPackage np = m_sendingPacks[0];
                net.socket.Send(dest, np.data, np.size);

                //	verify id the message has safe send option
                if ((np.header.option & net.NET_OPTN_SAFESEND) > 0)
                {
                    confirmNumber = np.header.number;
                    confirmChecksum = np.header.checksum;
                }
                else
                {
                    m_sendingPacks.RemoveAt(0);
                }
            }
        }
    }

    public void AppendReceivedMessage(byte[] buffer, int size)
    {
        outTime = 0;
        isConnected = true;

        byte[] headerBuffer = new byte[net.NET_HEADER_SIZE];

        Buffer.BlockCopy(buffer, 0, headerBuffer, 0, net.NET_HEADER_SIZE);

        NetHeader nh = net.ByteArrayToNetHeader(buffer);

        //	we are waiting for received confirmation from the other side
        if ((nh.option & net.NET_OPTN_CONFIRMED) > 0)
        {
            if (nh.number == confirmNumber && nh.checksum == confirmChecksum)
            {
                //sx_print(L"Info: Message number %d has been confirmed from other side!\n", confirmNumber);

                confirmNumber = 0;  // confirmed
                retryTime = 0;

                // remove the message from the sending list
                m_sendingPacks.RemoveAt(0);
            }
            else
            {
                // do nothing !!
            }
        }
        else
        {
            //	replay to sender that safe message has been received
            if ((nh.option & net.NET_OPTN_SAFESEND) > 0)
            {
                NetPackage tmp = new NetPackage(net.NET_HEADER_SIZE, net.NET_BUFF_SIZE);
                tmp.header = nh;
                tmp.header.option = net.NET_OPTN_CONFIRMED;
                byte[] tmpBuf = net.NetHeaderToByteArray(tmp.header);
                Buffer.BlockCopy(tmpBuf, 0, tmp.data, 0, net.NET_HEADER_SIZE);
                net.socket.Send(dest, tmp.data, tmp.size);
            }

            //	create message container and append it to the list
            if (nh.number != recvNumber)
            {
                recvNumber = nh.number;

                NetPackage np = new NetPackage(net.NET_HEADER_SIZE, net.NET_BUFF_SIZE);
                np.size = size;

                np.header = nh;

                Buffer.BlockCopy(buffer, 0, np.data, 0, size);

                //	add it to the received list
                m_receivedPacks.Add(np);

                //sx_print( L"message received header [NO: %d OP: %d CH: %d]\n", np.header.number, np.header.option, np.header.checksum );
            }
        }
    }
}
