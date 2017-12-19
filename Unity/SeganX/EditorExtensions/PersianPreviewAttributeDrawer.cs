#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

[CustomPropertyDrawer(typeof(PersianPreviewAttribute))]
public class PersianPreviewAttributeDrawer : PropertyDrawer
{
    private static GUIStyle style = new GUIStyle(GUI.skin.label)
    {
        alignment = TextAnchor.UpperRight,
        wordWrap = true,
        border = new RectOffset(10, 10, 0, 0),
        padding = new RectOffset(10, 15, 0, 0)
    };

    private float baseHeight;
    private float previewHeight;

    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        baseHeight = base.GetPropertyHeight(property, label);
        previewHeight = attribute.As<PersianPreviewAttribute>().height;
        if (previewHeight == 0) previewHeight = baseHeight;
        return baseHeight + previewHeight;
    }

    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        position.height = baseHeight;
        EditorGUI.PropertyField(position, property);

        var persianStr = PersianTextShaper.PersianTextShaper.ShapeText(property.stringValue.Replace('ي', 'ی'));
        position.y += baseHeight;
        position.height = previewHeight;
        EditorGUI.LabelField(position, persianStr, style);
    }
}
#endif