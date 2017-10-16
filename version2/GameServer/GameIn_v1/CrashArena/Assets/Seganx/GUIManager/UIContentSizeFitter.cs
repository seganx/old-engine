using UnityEngine;

namespace SeganX
{
    [ExecuteInEditMode]
    public class UIContentSizeFitter : MonoBehaviour
    {
        [System.Flags]
        public enum Option : int
        {
            Automatic = 0x01,
            ItemPosition = 0x02,
            ContentSize = 0x04
        }

        [EnumFlag]
        public Option options;

        public float padding = 0;
        public float space = 0;

        private void Perform()
        {
            if (options.IsFlagOn(Option.ContentSize))
                FitSize();
            else if (options.IsFlagOn(Option.ItemPosition))
                UpdatePositions();
        }

        public float UpdatePositions()
        {
            float height = padding;
            for (int i = 0; i < transform.childCount; i++)
            {
                RectTransform rt = transform.GetChild(i) as RectTransform;
                if (rt.gameObject.activeInHierarchy)
                {
                    var pos = rt.anchoredPosition;
                    pos.y = rt.anchorMax.y > 0.5f ? -height : height;
                    rt.anchoredPosition = pos;
                    height += rt.rect.height + space;
                }
            }
            height -= space;
            return height + padding;
        }

        public void FitSize()
        {
            transform.SetAnchordHeight(UpdatePositions());
        }

        void Update()
        {
#if UNITY_EDITOR
            Perform();
#else
            if (options.IsFlagOn(Option.Automatic))
                Perform();
#endif
        }
    }
}
