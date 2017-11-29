using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class UIProgressBar : MonoBehaviour
    {
        private float initwidth = 0;

        void Awake()
        {
            initwidth = transform.As<RectTransform>().rect.width;
        }

        public void Set(float value, float maxValue)
        {
            transform.SetAnchordWidth(value * initwidth / maxValue);
        }
    }
}

