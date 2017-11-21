using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

[CreateAssetMenu(menuName = "Game/Character")]
public class CharacterData : AssetData
{
    public string nickName = "character";

    public List<Body> bodies = new List<Body>();
    public List<Face> faces = new List<Face>();
    public List<Hair> hairs = new List<Hair>();
}
