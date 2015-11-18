using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Net;


public class Connection
{
    public List<NetPacket> m_sendingPacks = new List<NetPacket>();	//!	list of packages for send
    public List<NetPacket> m_receivedPacks = new List<NetPacket>();	//!	list of received packages


    bool isConnected = true;		    //! specified that the connection is established
    public IPEndPoint dest;		        //! connection address

    ushort sentNumber = 1;              //! used in reliability system. hold the number of message sent
    public ushort recvNumber = 0;       //! used in reliability system. hold the number of latest message has been received
                                        
    ushort confirmNumber = 0;           //! indicates that connection is waiting for received confirmation
    ushort confirmChecksum = 0;         //! indicates that connection is waiting for received confirmation
                                        
    double delayTime = 0;		        //!	delay time counter
    double delayTimeMax = 0.1f;         //! maximum delay time
                                        
    double retryTime = 0;		        //!	retry time counter
    double retryTimeMax = 1;        	//! maximum retry time

    double outTime = 0;			        //!	time out counter
    double outTimeMax = 10;		        //! maximum time out


    public void Open(IPEndPoint destination)
    {
        dest = destination;
    }

    void Close()
    {
        //	clear sending list
        m_sendingPacks.Clear();

        //	clear received list
        m_receivedPacks.Clear();

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
        if ( buffer.Length < 0 ) return false;
        if (buffer.Length + NetBase.NET_HEADER_SIZE > NetBase.NET_BUFF_SIZE) return false;

        if (sentNumber < 1) sentNumber++;

        NetPacket np = new NetPacket();
        np.header.netId = NetBase.NET_ID;
        np.header.option = option;
        np.header.number = sentNumber++;
        np.header.checksum = (ushort)NetBase.ComputeChecksum(buffer);
        np.data = buffer;

        m_sendingPacks.Add(np);

        return true;
    }

    public int Received(out byte[] destBuffer)
    {
        if (m_receivedPacks.Count > 0)
        {
            NetPacket np = m_receivedPacks[0];

            //	remove from the list
            m_receivedPacks.RemoveAt(0);

            destBuffer = np.data;
            return np.data.Length;
        }
        else
        {
            destBuffer = null;
            return 0;
        }
    }

    public void Update(NetSocket socket, double elpsTime)
    {
        if (confirmNumber > 0)	//	we are waiting for received confirmation from the other side
        {
            //	check retry time out and resend the message
            retryTime += elpsTime;
            if (retryTime > retryTimeMax)
            {
                retryTime = 0;

                if (m_sendingPacks.Count > 0)
                {
                    socket.Send(m_sendingPacks[0], dest);
                }
                else
                {
                    confirmNumber = 0;  // confirmed
                    retryTime = 0;
                    outTime = 0;
                }
            }

            //	check connection time out to close the connection
            outTime += elpsTime;
            if (outTime > outTimeMax)
            {
                // the connection is dropped
                outTime = 0;
                isConnected = false;
                Debug.LogWarning("Connection time out");
            }
        }
        else // just continue sending data
        {
            if (delayTimeMax > 0)
            {
                delayTime += elpsTime;
                if (delayTime < delayTimeMax) return;
                delayTime = 0;
            }

            while (m_sendingPacks.Count > 0 && confirmNumber == 0)	// is there any message to send ?
            {
                NetPacket np = m_sendingPacks[0];

                //  send packet to the destination
                socket.Send(np, dest);

                //	verify id the message has safe send option
                if ((np.header.option & NetBase.NET_OPTN_SAFESEND) > 0)
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

    public void AppendReceivedMessage(NetSocket socket, byte[] buffer, int size)
    {
        outTime = 0;
        isConnected = true;

        NetHeader nh = NetBase.ByteArrayToNetHeader(buffer);

        //	we are waiting for received confirmation from the other side
        if ((nh.option & NetBase.NET_OPTN_CONFIRMED) > 0)
        {
            if (nh.number == confirmNumber && nh.checksum == confirmChecksum)
            {
                Debug.Log("Info: Message number has been confirmed from other side!");

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
            if ((nh.option & NetBase.NET_OPTN_SAFESEND) > 0)
            {
                NetPacket tmp = new NetPacket();
                tmp.header = nh;
                tmp.header.option = NetBase.NET_OPTN_CONFIRMED;
                socket.Send(tmp, dest);
            }

            //	create message container and append it to the list
            if (nh.number != recvNumber)
            {
                recvNumber = nh.number;

                NetPacket np = new NetPacket();
                np.header = nh;
                np.data = new byte[size - NetBase.NET_HEADER_SIZE];

                Buffer.BlockCopy(buffer, NetBase.NET_HEADER_SIZE, np.data, 0, np.data.Length);

                //	add it to the received list
                m_receivedPacks.Add(np);

                //sx_print( L"message received header [NO: %d OP: %d CH: %d]\n", np.header.number, np.header.option, np.header.checksum );
            }
        }
    }
}
