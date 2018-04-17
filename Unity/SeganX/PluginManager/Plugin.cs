using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "Plugin", menuName = "Game/Plugin")]
public class Plugin : ScriptableObject
{
    public bool activated = false;

    public string folder = "";
    [TextArea(1, 10)]
    public string symbols = "";

    [TextArea(5, 10)]
    public string description = "";

    public List<string> files = new List<string>();
}
