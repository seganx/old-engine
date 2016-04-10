using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Text;

public class NetworkTest : NetBase
{
    [System.Serializable()]
    public class Server
    {
        public string IP;
        public int Port;
        [HideInInspector]
        public Connection conn;
    }

    public Server[] servers;

    public void Login()
    {
        Server server = servers[0];

        if (server.conn == null)
            server.conn = CreateConnection(new IPEndPoint(IPAddress.Parse(server.IP), server.Port));

        server.conn.Send(Encoding.Unicode.GetBytes("Login"), NET_OPTN_SAFESEND);
    }
    public void SendMessage(string str)
    {
        Server server = servers[0];

        if (server.conn == null)
            server.conn = CreateConnection(new IPEndPoint(IPAddress.Parse(server.IP), server.Port));

        server.conn.Send(Encoding.Unicode.GetBytes(str), NET_OPTN_SAFESEND);
    }
    public void Login2()
    {
        Server server = servers[1];

        if (server.conn == null)
            server.conn = CreateConnection(new IPEndPoint(IPAddress.Parse(server.IP), server.Port));

        server.conn.Send(Encoding.Unicode.GetBytes("Login"), NET_OPTN_SAFESEND);
    }

    public void GetProfile()
    {
        Server server = servers[0];

        if (server.conn == null)
            server.conn = CreateConnection(new IPEndPoint(IPAddress.Parse(server.IP), server.Port));

        server.conn.Send(Encoding.ASCII.GetBytes("Login"), NET_OPTN_SAFESEND);
    }
}
