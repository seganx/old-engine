using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(BrushPrefab))]
public class BrushPrefabEditor : Editor
{
    void OnSceneGUI()
    {
        var brushMan = target as BrushPrefab;
        if (brushMan == null || brushMan.prefab == null || brushMan.editMode == BrushPrefab.EditMode.Select)
            return;

        Event evt = Event.current;
        if (evt.button != 0) return;
        if (evt.modifiers != EventModifiers.None && evt.modifiers != EventModifiers.Shift) return;

        switch (evt.type)
        {
            case EventType.MouseUp:
                {
                    Ray ray = HandleUtility.GUIPointToWorldRay(Event.current.mousePosition);
                    RaycastHit hit;
                    if (Physics.Raycast(ray, out hit))
                    {
                        var newone = Instantiate(brushMan.prefab, hit.point, Quaternion.identity, brushMan.transform);
                        newone.SendMessage("OnCreatedByBrushPrefab", evt, SendMessageOptions.DontRequireReceiver);
                        BrushPrefab.lastCreated = newone;
                    }
                    evt.Use();
                }
                break;

            case EventType.Layout:
                {
                    int id = GUIUtility.GetControlID(FocusType.Passive);
                    HandleUtility.AddDefaultControl(id);
                }
                break;
        }
    }
}
