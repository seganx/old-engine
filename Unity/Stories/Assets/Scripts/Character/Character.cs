﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class Character : Base
{
    public static List<AssetBundle> boundles = new List<AssetBundle>();

    public CharacterData test = null;
    public Body body = null;
    public Face face = null;
    public Hair hair = null;

    public Character Setup(Body bodyPrefab, Face facePrefab, Hair hairPrefab)
    {
        if (body) Destroy(body.gameObject);

        body = bodyPrefab.Clone<Body>();
        face = facePrefab.Clone<Face>();
        hair = hairPrefab.Clone<Hair>();

        if (body) body.transform.SetParent(transform, false);
        if (face && body) face.transform.SetParent(body.faceJoint, false);
        if (hair && face) hair.transform.SetParent(face.hairJoint, false);

        return this;
    }

    public Character Setup(string bodyName, string faceName, string hairName)
    {
        if (body) Destroy(body.gameObject);

        body = LoadBody(bodyName);
        face = LoadFace(faceName);
        hair = LoadHair(hairName);

        if (body) body.transform.SetParent(transform, false);
        if (face && body) face.transform.SetParent(body.faceJoint, false);
        if (hair && face) hair.transform.SetParent(face.hairJoint, false);

        return this;
    }

    public Character SetupBody(string bodyName)
    {
        if (face) face.transform.SetParent(null, false);
        if (body) Destroy(body.gameObject);
        body = LoadBody(bodyName);
        if (body) body.transform.SetParent(transform, false);
        if (face && body) face.transform.SetParent(body.faceJoint, false);
        return this;
    }

    public Character SetupFace(string faceName)
    {
        if (hair) hair.transform.SetParent(null, false);
        if (face) Destroy(face.gameObject);
        face = LoadFace(faceName);
        if (face && body) face.transform.SetParent(body.faceJoint, false);
        if (hair && face) hair.transform.SetParent(face.hairJoint, false);
        return this;
    }

    public Character SetupHair(string hairName)
    {
        if (hair) Destroy(hair.gameObject);
        hair = LoadHair(hairName);
        if (hair && face) hair.transform.SetParent(face.hairJoint, false);
        return this;
    }


    private void Start()
    {
        if (test != null)
        {
            Setup(test.bodies[0], test.faces[0], test.hairs[0]);
        }
        else
        {
            LoadFromCacheOrDownload("http://locator.8khan.ir/Tests/Assets/data.data", 1, ws =>
            {
                test = AssetData.Load(ws) as CharacterData;
                Setup(test.bodies[0], test.faces[0], test.hairs[0]);
                Debug.Log(test);
                //SetupBody(test.bodies[0]);
            });
        }
    }

    //////////////////////////////////////////////////////////////////
    //  STATIC MEMBERS
    //////////////////////////////////////////////////////////////////
    public static Body LoadBody(string name)
    {
        var go = Resources.Load<Body>("Prefabs/Characters/Faces/" + name).Clone<Body>();
        go.name = name;
        return go;
    }

    public static Face LoadFace(string name)
    {
        var go = Resources.Load<Face>("Prefabs/Characters/Faces/" + name).Clone<Face>();
        go.name = name;
        return go;
    }

    public static Hair LoadHair(string name)
    {
        var go = Resources.Load<Hair>("Prefabs/Characters/Hairs/" + name).Clone<Hair>();
        go.name = name;
        return go;
    }
}
