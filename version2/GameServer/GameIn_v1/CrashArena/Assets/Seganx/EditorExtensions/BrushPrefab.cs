using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BrushPrefab : MonoBehaviour
{
#if UNITY_EDITOR
    public static GameObject lastCreated = null;

    public GameObject prefab = null;

    public enum EditMode
    {
        Select,
        Create
    }
    [EnumToggle]
    public EditMode editMode;
#endif
}
