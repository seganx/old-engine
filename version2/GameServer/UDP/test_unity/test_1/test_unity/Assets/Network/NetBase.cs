using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Threading;
using System.Text;
using System.Net;
using System.Net.Sockets;


//! describe packet's header
public struct NetHeader
{
    public ushort netId;         //! net id helps to avoid servers conflicts
    public ushort number;        //! message number
    public ushort option;        //! message option contains message flag
    public ushort checksum;      //! checksum of the content data
};

//! describe a packet content
public class NetPacket
{
    public NetHeader header;    //! no allocation needed. It just points to data
    public byte[] data;	        //! it also contains header
}

public class NetBase : MonoBehaviour
{
    public static int NET_BUFF_SIZE = 1024;	            //! maximum size of a package in bytes
    public static ushort NET_OPTN_SAFESEND = 0x0001;	//! used in message header and indicates that the message should be resend till other sides confirms
    public static ushort NET_OPTN_CONFIRMED = 0x0002;	//! used in message header and indicates that the message is confirmation by the other sides
    public static int NET_HEADER_SIZE = 8;
    public static ushort NET_ID = 8;

    public ushort netId = 0;

    void Start()
    {
        NET_ID = netId;
    }

    public static byte[] NetHeaderToByteArray(NetHeader header)
    {
        byte[] buffer = new byte[NET_HEADER_SIZE];
        Buffer.BlockCopy(BitConverter.GetBytes(header.netId), 0, buffer, 0, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.number), 0, buffer, 2, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.option), 0, buffer, 4, 2);
        Buffer.BlockCopy(BitConverter.GetBytes(header.checksum), 0, buffer, 6, 2);
        return buffer;
    }

    public static NetHeader ByteArrayToNetHeader(byte[] data)
    {
        NetHeader nh = new NetHeader();
        nh.netId = BitConverter.ToUInt16(data, 0);
        nh.number = BitConverter.ToUInt16(data, 2);
        nh.option = BitConverter.ToUInt16(data, 4);
        nh.checksum = BitConverter.ToUInt16(data, 6);
        return nh;
    }

    public static ushort ComputeChecksum(byte[] buffer)
    {
        ushort sum1 = 0;
        ushort sum2 = 0;
        for (int i = 0; i < buffer.Length; ++i)
        {
            ushort tmp = sum1;
            tmp += buffer[i];
            tmp %= 255;
            sum1 = tmp;

            tmp = sum2;
            tmp += sum1;
            tmp %= 255;
            sum2 = tmp;
        }

        ushort res = (ushort)(sum2 << 8);
        res |= sum1;
        return res;
    }

    public static bool VerifyPackage(byte[] buffer, int size, ulong lastNumber)
    {
        //	validate message size
        if (size < NET_HEADER_SIZE || size > NET_BUFF_SIZE)
            return false;

        NetHeader nh = ByteArrayToNetHeader(buffer);

        //	validate net id
        if (nh.netId != NET_ID)
            return false;

        //	validate if message is duplicated
        if (nh.number == lastNumber && (nh.option & NET_OPTN_CONFIRMED) == 0)
            return false;

        //	validate data checksum
        if (size > NET_HEADER_SIZE)
        {
            if (nh.checksum != ComputeChecksum(buffer))
                return false;
        }

        // new we can suppose that the package is valid
        return true;
    }

}

