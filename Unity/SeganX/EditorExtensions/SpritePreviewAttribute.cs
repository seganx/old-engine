﻿using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace SeganX
{
    public class SpritePreviewAttribute : PropertyAttribute
    {
        public float height = 0;
        public SpritePreviewAttribute() { }
        public SpritePreviewAttribute(int previewHeight)
        {
            height = previewHeight;
        }
    }

#if UNITY_EDITOR
    [CustomPropertyDrawer(typeof(SpritePreviewAttribute))]
    public class SpritePreviewAttributeDrawer : PropertyDrawer
    {
        private static Texture2D s_currentTexture = null;
        private static Texture2D s_currentTextureConverted = null;

        private float baseHeight;
        private float previewHeight;

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            baseHeight = base.GetPropertyHeight(property, label);
            previewHeight = attribute.As<SpritePreviewAttribute>().height;
            if (previewHeight == 0)
            {
                //var sprite = property.objectReferenceValue.IsTypeOf<Sprite>() ? property.objectReferenceValue as Sprite : null;
                var sprite = property.objectReferenceValue as Sprite;
                if (sprite != null) previewHeight = sprite.rect.height;
            }
            return baseHeight + previewHeight;
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            position.height = baseHeight;
            EditorGUI.PropertyField(position, property);

            if (property.objectReferenceValue.IsTypeOf<Sprite>())
            {
                position.y += baseHeight;
                position.x += EditorGUIUtility.labelWidth;
                position.width -= EditorGUIUtility.labelWidth;
                position.height = previewHeight;

                var sprite = property.objectReferenceValue.As<Sprite>();
                if (s_currentTexture != sprite.texture)
                {
                    s_currentTexture = sprite.texture;
                    s_currentTextureConverted = new Texture2D(s_currentTexture.width, s_currentTexture.height, TextureFormat.ARGB32, false);
                    Graphics.ConvertTexture(s_currentTexture, 0, s_currentTextureConverted, 0);
                }

                var texture = new Texture2D((int)sprite.rect.width, (int)sprite.rect.height, TextureFormat.ARGB32, false);
                try
                {
                    Graphics.CopyTexture(s_currentTextureConverted, 0, 0, (int)sprite.rect.x, (int)sprite.rect.y, texture.width, texture.height, texture, 0, 0, 0, 0);
                }
                catch
                {
                    s_currentTextureConverted = null;
                }
                EditorGUI.LabelField(position, new GUIContent() { image = texture });
            }
        }

        private void DrawTexturePreview(Rect position, Sprite sprite)
        {
            if (sprite == null) return;

            Vector2 fullSize = new Vector2(sprite.texture.width, sprite.texture.height);
            Vector2 size = new Vector2(sprite.textureRect.width, sprite.textureRect.height);

            Rect coords = sprite.textureRect;
            coords.x /= fullSize.x;
            coords.width /= fullSize.x;
            coords.y /= fullSize.y;
            coords.height /= fullSize.y;

            Vector2 ratio;
            ratio.x = position.width / size.x;
            ratio.y = position.height / size.y;
            float minRatio = Mathf.Min(ratio.x, ratio.y);

            Vector2 center = position.center;
            position.width = size.x * minRatio;
            position.height = size.y * minRatio;
            position.center = center;

            GUI.DrawTextureWithTexCoords(position, sprite.texture, coords);
        }
    }
#endif
}