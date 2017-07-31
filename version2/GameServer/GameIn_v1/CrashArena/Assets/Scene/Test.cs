using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class Test : MonoBehaviour
{
    public TextAsset jsonFile = null;

    // Use this for initialization
    IEnumerator Start()
    {
        yield return new WaitForSeconds(0.5f);

        while (true)
        {
            var root = JSON.Parse(jsonFile.text);
            Debug.Log(root.ToString());
            HandleJsonObject(root);
            yield return new WaitForSeconds(20);
        }
    }

    void HandleJsonObject(JSONNode root)
    {
        var obj = Resources.Load("Machines/Machine");

        var machine = obj.Clone<Machine>();
        machine.Setup(root, Side.Player);
        machine.transform.localPosition = Vector3.left * 2;

        machine = obj.Clone<Machine>();
        machine.Setup(root, Side.Opponent);
        machine.transform.localPosition = Vector3.right * 2;

        //UnityEditor.EditorApplication.isPaused = true;
    }

    // Update is called once per frame
    void Update()
    {

    }

    [Console("game")]
    public static void MyTest(int x, float y, string s)
    {
        Debug.Log("my test: " + x + " " + y + " " + s);
    }

    [Console("game")]
    public static void JustTest()
    {
        Debug.Log("just test");
    }
}
