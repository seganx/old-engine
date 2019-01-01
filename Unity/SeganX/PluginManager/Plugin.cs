using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "Plugin", menuName = "Game/Plugin")]
public class Plugin : ScriptableObject
{
    public bool activated = false;

    public string folder = string.Empty;
    [TextArea(1, 10)]
    public string symbols = string.Empty;

    [TextArea(5, 10)]
    public string description = string.Empty;

    public List<string> files = new List<string>();
}
