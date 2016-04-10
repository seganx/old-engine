using UnityEngine;
using System.Collections;

public class Test : MonoBehaviour
{
    public NetworkTest network;

    // Use this for initialization
    void Start()
    {

    }

    int counter = 5;

    // Update is called once per frame
    void Update()
    {
        for (int i = 0; i < counter / 5; i++)
        {
            network.SendMessage("Counter: " + counter.ToString());
            counter++;
        }
    }

    void OnGUI()
    {
        if (GUI.Button(new Rect(10, 10, 50, 50), "Login"))
        {
            network.Login();
            //network.Login2();
        }
    }
}
