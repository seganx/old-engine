#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;
using System.Reflection;

namespace SeganX
{
    [System.AttributeUsage(System.AttributeTargets.Field)]
    public class InspectorButtonAttribute : PropertyAttribute
    {
        public readonly float ButtonWidth = 100;
        public readonly string[] ButtonNames;
        public readonly string[] MethodNames;

        public InspectorButtonAttribute(float width, string buttonName, string methodName)
        {
            ButtonWidth = width;
            this.MethodNames = new string[1] { methodName };
            this.ButtonNames = new string[1] { buttonName };
        }
        public InspectorButtonAttribute(float width, string buttonName1, string methodName1, string buttonName2, string methodName2)
        {
            ButtonWidth = width;
            this.MethodNames = new string[2] { methodName1, methodName2 };
            this.ButtonNames = new string[2] { buttonName1, buttonName2 };
        }
        public InspectorButtonAttribute(float width, string buttonName1, string methodName1, string buttonName2, string methodName2, string buttonName3, string methodName3)
        {
            ButtonWidth = width;
            this.MethodNames = new string[3] { methodName1, methodName2, methodName3 };
            this.ButtonNames = new string[3] { buttonName1, buttonName2, buttonName3 };
        }
        public InspectorButtonAttribute(float width, string buttonName1, string methodName1, string buttonName2, string methodName2, string buttonName3, string methodName3, string buttonName4, string methodName4)
        {
            ButtonWidth = width;
            this.MethodNames = new string[4] { methodName1, methodName2, methodName3, methodName4 };
            this.ButtonNames = new string[4] { buttonName1, buttonName2, buttonName3, buttonName4 };
        }
    }

    [CustomPropertyDrawer(typeof(InspectorButtonAttribute))]
    public class InspectorButtonPropertyDrawer : PropertyDrawer
    {
        void CallMethod(SerializedProperty prop, string eventName)
        {
            System.Type eventOwnerType = prop.serializedObject.targetObject.GetType();
            var eventMethodInfo = eventOwnerType.GetMethod(eventName, BindingFlags.Instance | BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic);
            if (eventMethodInfo != null)
                eventMethodInfo.Invoke(prop.serializedObject.targetObject, new object[1] { prop.propertyPath });
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            return base.GetPropertyHeight(property, label) * 2;
        }

        public override void OnGUI(Rect position, SerializedProperty prop, GUIContent label)
        {
            position.height /= 2;
            InspectorButtonAttribute inspectorButtonAttribute = (InspectorButtonAttribute)attribute;
            var n = inspectorButtonAttribute.MethodNames.Length - 1;
            float xOffset = n < 1 ? 0 : (position.width - inspectorButtonAttribute.ButtonWidth) / n;
            for (int i = 0; i < inspectorButtonAttribute.MethodNames.Length; i++)
            {
                Rect buttonRect = new Rect(position.x + xOffset * i, position.y, inspectorButtonAttribute.ButtonWidth, position.height);
                if (GUI.Button(buttonRect, inspectorButtonAttribute.ButtonNames[i]))
                    CallMethod(prop, inspectorButtonAttribute.MethodNames[i]);
            }

            position.y += position.height;
            EditorGUI.PropertyField(position, prop);
        }
    }
}
#endif
