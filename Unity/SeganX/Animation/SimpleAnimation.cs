using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class SimpleAnimation : MonoBehaviour
    {
        public Animation target = null;
        private List<AnimationState> animationStates = new List<AnimationState>();
        private int lastPlayed = -1;

        private void Reset()
        {
            if (target == null) target = transform.GetComponent<Animation>(true, true);
        }

        private void Awake()
        {
            foreach (AnimationState state in target)
                animationStates.Add(state);
        }

        public SimpleAnimation PlayByIndex(int index)
        {
            if (lastPlayed != index)
            {
                lastPlayed = index;
                target.Play(animationStates[index].name);
            }

            return this;
        }
    }
}
