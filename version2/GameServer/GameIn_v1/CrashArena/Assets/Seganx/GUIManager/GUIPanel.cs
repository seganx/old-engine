using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public abstract class GUIPanel : Base
    {
        private RectTransform content = null;

        protected RectTransform Content
        {
            get
            {
                if (content == null)
                    content = transform.Find("Content") as RectTransform;
                return content;
            }
        }

        public virtual void Reset()
        {
            gameObject.name = GetType().Name;
        }

        public virtual void ChangeZOrder(float z)
        {
            if (Content)
            {
                Vector3 pos = Content.anchoredPosition3D;
                pos.z += (z - pos.z) * Time.deltaTime * 5;
                Content.anchoredPosition3D = pos;
            }
        }

        public abstract void Back();
    }
}
