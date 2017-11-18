using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[CreateAssetMenu(menuName = "Game/Character")]
public class CharacterData : ScriptableObject
{
    public int version = 1;
    public List<string> bodies = new List<string>();
    public List<string> faces = new List<string>();
    public List<string> hairs = new List<string>();
}
