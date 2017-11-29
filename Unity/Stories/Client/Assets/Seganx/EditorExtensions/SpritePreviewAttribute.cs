using UnityEngine;
public class SpritePreviewAttribute : PropertyAttribute
{
    public float height = 0;
    public SpritePreviewAttribute() { }
    public SpritePreviewAttribute(int previewHeight)
    {
        height = previewHeight;
    }
}