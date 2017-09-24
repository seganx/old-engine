using UnityEngine;
using System;

namespace SeganX
{
    [Serializable]
    public class Value
    {
        [SerializeField]
        private float min = 0;

        [SerializeField]
        private float max = 0;

        [SerializeField]
        private float current = 0;

        public bool IsMax { get { return Mathf.Approximately(current, max); } }
        public bool IsMin { get { return Mathf.Approximately(current, min); } }
        public bool IsZero { get { return Mathf.Approximately(current, 0); } }

        public float Min
        {
            set
            {
                min = value;
                if (min > current) current = min;
                if (min > max) max = min;
            }
            get { return min; }
        }

        public float Max
        {
            set
            {
                max = value;
                if (max < current) current = max;
                if (max < min) min = max;
            }
            get { return max; }
        }

        public float Current
        {
            set { current = Mathf.Clamp(value, min, max); }
            get { return current; }
        }

        public float Percent
        {
            get
            {
                float d = (max - min);
                return (d > Mathf.Epsilon) ? current * 100 / d : 0;
            }
            set { current = value * 0.01f * (max - min); }            
        }
    }
}
