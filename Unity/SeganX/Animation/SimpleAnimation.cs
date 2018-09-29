using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class SimpleAnimation : MonoBehaviour
    {
        [System.Serializable]
        public class AnimationData
        {
            public string name = "";
            public int id = 0;
        }

        public Animation target = null;
        public List<AnimationData> animations = new List<AnimationData>();
        private AnimationState lastState = null;
        private int lastPlayed = -1;

        public AnimationState PlayById(int id, bool once = true)
        {
            var res = animations.Find(x => x.id == id);
            if (res == null) return null;

            if (!once || lastPlayed != id)
            {
                lastPlayed = id;
                lastState = target[res.name];
                target.CrossFade(res.name, 0.1f, PlayMode.StopAll);
            }
            return lastState;
        }

#if UNITY_EDITOR
        private void Reset()
        {
            if (target == null)
                target = this.GetComponent<Animation>(true, true);

            lastPlayed = 0;
            animations.Clear();
            foreach (AnimationState state in target)
            {
                var data = new AnimationData();
                data.name = state.clip.name;
                data.id = lastPlayed++;
                animations.Add(data);
            }
        }
#endif        
    }
}
