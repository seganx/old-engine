using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class UIProgressBar : Base
    {
        private float initwidth = 0;

        void Start()
        {
            Setup();
        }

        public UIProgressBar Setup()
        {
            initwidth = rectTransform.rect.width;
            return this;
        }

        public void Set(float value, float maxValue)
        {
            var width = Mathf.Clamp(value * initwidth / maxValue, 0, initwidth);
            transform.SetAnchordWidth(width);
        }
    }
}

