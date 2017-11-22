using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

[CreateAssetMenu(menuName = "Game/Character")]
public class AssetCharacter : AssetData
{
    public string family = "character";

    public List<Body> bodies = new List<Body>();
    public List<Face> faces = new List<Face>();
    public List<Hair> hairs = new List<Hair>();
    public List<Body> bodiesFL = new List<Body>();

    public static AssetCharacter FindCharacter(string family)
    {
        return s_assets.Find(x => x is AssetCharacter && x.As<AssetCharacter>().family == family).As<AssetCharacter>();
    }

    public static Character Setup(Character dest, CharacterData data, bool fullLength = false)
    {
        var src = FindCharacter(data.family);
        if (src)
        {
            Body body = null;
            if (fullLength)
                body = src.bodiesFL.Find(x => x.name.ToLower() == data.body.ToLower()) ?? src.bodiesFL[0];
            else
                body = src.bodies.Find(x => x.name.ToLower() == data.body.ToLower()) ?? src.bodies[0];

            var face = src.faces.Find(x => x.name.ToLower() == data.face.ToLower()) ?? src.faces[0];
            var hair = src.hairs.Find(x => x.name.ToLower() == data.hair.ToLower()) ?? src.hairs[0];
            return dest.Setup(body, face, hair);
        }
        else return null;
    }
}
