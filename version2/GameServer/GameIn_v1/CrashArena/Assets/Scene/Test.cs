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

        DelayCall(1, Clone);
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

        //var obj = Resources.Load("Machines/Machine");

        var machine = Machine.Create();
        machine.Setup(root, MachineParty.Player);
        machine.transform.localPosition = Vector3.left * 4.5f;

        machine = Machine.Create();
        machine.Setup(root, MachineParty.Opponent);
        machine.transform.localPosition = Vector3.right * 4.5f;

        //UnityEditor.EditorApplication.isPaused = true;
    }
}
