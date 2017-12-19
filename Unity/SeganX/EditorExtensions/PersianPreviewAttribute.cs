using UnityEngine;
public class PersianPreviewAttribute : PropertyAttribute
{
    public float height = 0;
    public PersianPreviewAttribute() { }
    public PersianPreviewAttribute(int previewHeight)
    {
        height = previewHeight;
    }
}