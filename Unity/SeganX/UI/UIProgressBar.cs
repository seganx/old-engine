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
            initwidth = transform.As<RectTransform>().rect.width;
            return this;
        }

        public void Set(float value, float maxValue)
        {
            transform.SetAnchordWidth(value * initwidth / maxValue);
        }
    }
}

