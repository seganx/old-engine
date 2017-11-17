using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[CreateAssetMenu(menuName = "Game/Character")]
public class CharacterData : ScriptableObject
{
    public List<string> bodies = new List<string>();
    public List<string> faces = new List<string>();
    public List<string> hairs = new List<string>();
}
