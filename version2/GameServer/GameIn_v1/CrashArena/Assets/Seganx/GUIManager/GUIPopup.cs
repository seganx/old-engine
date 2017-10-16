using UnityEngine;

namespace SeganX
{
    public class GUIPopup : GUIPanel
    {
        public bool pushOthersDown = false;

        public virtual void Awake()
        {
            if (Content && pushOthersDown)
                Content.anchoredPosition3D = Vector3.back * 500;
        }

        public override void Back()
        {
            guiManager.ClosePopup(this);
        }
    }
}