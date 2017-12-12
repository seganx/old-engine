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

    public Character Setup(CharacterData data, bool fullLength = false)
    {
        Clear();

        var src = AssetCharacter.FindCharacter(data.family);
        if (src == null) return this;

        Body body = null;
        if (fullLength)
            body = src.bodiesFL.Find(x => x.name.ToLower() == data.body.ToLower()) ?? src.bodiesFL[0];
        else
            body = src.bodies.Find(x => x.name.ToLower() == data.body.ToLower()) ?? src.bodies[0];

        var face = src.faces.Find(x => x.name.ToLower() == data.face.ToLower()) ?? src.faces[0];
        var hair = src.hairs.Find(x => x.name.ToLower() == data.hair.ToLower()) ?? src.hairs[0];

        return Setup(body, face, hair);
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
}
