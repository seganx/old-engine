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

    public CharacterData Default { get { return new CharacterData(family, name, bodies[0].name, faces[0].name, hairs[0].name); } }

    public static AssetCharacter FindCharacter(string family)
    {
        return s_assets.Find(x => x is AssetCharacter && x.As<AssetCharacter>().family == family).As<AssetCharacter>();
    }

    public static List<AssetCharacter> FindAll()
    {
        var res = new List<AssetCharacter>();
        var r = s_assets.FindAll(x => x is AssetCharacter);
        foreach (var item in r)
            res.Add(item as AssetCharacter);
        return res;
    }
}
