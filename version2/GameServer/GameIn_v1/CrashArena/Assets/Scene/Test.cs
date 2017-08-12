using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using SeganX;

public class Test : Base
{

    public TextAsset jsonFile = null;

    // Use this for initialization
    void Start()
    {
        Download("http://locator.8khan.ir/Tests/Assets/Asset.xml", w =>
        {
            print("Downloaded: " + w.text);
        });

        LoadFromCacheOrDownload("http://locator.8khan.ir/Tests/Assets/Asset.xml", 1, w =>
        {
            print(w.text);
        });
    }

    // Update is called once per frame
    void Update()
    {

    }

    private static Test instance = null;
    public static Test Instance { get { return instance == null ? instance = FindObjectOfType<Test>() : instance; } }

    [SeganX.Console("game")]
    public static void Clone()
    {
        var root = Json.Parse(Instance.jsonFile.text);
        Debug.Log(root.ToString());

        var obj = Resources.Load("Machines/Machine");

        var machine = obj.Clone<Machine>();
        machine.Setup(root, Side.Player);
        machine.transform.localPosition = Vector3.left * 2;

        machine = obj.Clone<Machine>();
        machine.Setup(root, Side.Opponent);
        machine.transform.localPosition = Vector3.right * 2;

        //UnityEditor.EditorApplication.isPaused = true;
    }
}
