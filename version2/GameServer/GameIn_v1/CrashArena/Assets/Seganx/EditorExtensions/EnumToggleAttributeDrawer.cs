#if UNITY_EDITOR
using System;
using UnityEditor;
using UnityEngine;

[CustomPropertyDrawer(typeof(EnumToggleAttribute))]
public class EnumToggleAttributeDrawer : PropertyDrawer
{
    public override void OnGUI(Rect _position, SerializedProperty _property, GUIContent _label)
    {
        int enumLength = _property.enumNames.Length;
        float buttonWidth = (_position.width - EditorGUIUtility.labelWidth) / enumLength;

        EditorGUI.LabelField(new Rect(_position.x, _position.y, EditorGUIUtility.labelWidth, _position.height), _label);

        EditorGUI.BeginChangeCheck();

        int buttonsIntValue = _property.enumValueIndex;
        for (int i = 0; i < enumLength; i++)
        {
            Rect buttonPos = new Rect(_position.x + EditorGUIUtility.labelWidth + buttonWidth * i, _position.y, buttonWidth, _position.height);
            if (GUI.Toggle(buttonPos, buttonsIntValue == i, _property.enumNames[i], "Button"))
                buttonsIntValue = i;
        }

        if (EditorGUI.EndChangeCheck())
            _property.enumValueIndex = buttonsIntValue;
    }
}
#endif