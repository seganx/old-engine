using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class Character : Base
{
    public static List<AssetBundle> boundles = new List<AssetBundle>();

    public Body body = null;
    public Face face = null;
    public Hair hair = null;

    public void Clear()
    {
        if (body) Destroy(body.gameObject);
        body = null; face = null; hair = null;
    }

    public Character Setup(CharacterData data)
    {
        return Setup(data.body, data.face, data.hair);
    }

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
#if OFF
        var chData = new CharacterData() { family = "sajad", body = "body_2", face = "face_1", hair = "hair_3", name = "salman" };

        if (test != null)
        {
            Setup(test.bodies[0], test.faces[0], test.hairs[0]);
        }
        else
        {
            LoadFromCacheOrDownload("http://locator.8khan.ir/Tests/Assets/sajad.char.seganx", 1, ws =>
            {
                AssetData.LoadEncrypted(ws.bytes);
                AssetCharacter.Setup(this, chData, true);
            });
        }
#endif
    }

    //////////////////////////////////////////////////////////////////
    //  STATIC MEMBERS
    //////////////////////////////////////////////////////////////////
    public static Body LoadBody(string name)
    {
        name = name.ToLower();
        var go = gameManager.testCharacter.bodies.Find(x => x.name.ToLower() == name).Clone<Body>();
        go.name = name;
        return go;
    }

    public static Face LoadFace(string name)
    {
        name = name.ToLower();
        var go = gameManager.testCharacter.faces.Find(x => x.name.ToLower() == name).Clone<Face>();
        go.name = name;
        return go;
    }

    public static Hair LoadHair(string name)
    {
        name = name.ToLower();
        var go = gameManager.testCharacter.hairs.Find(x => x.name.ToLower() == name).Clone<Hair>();
        go.name = name;
        return go;
    }
}
