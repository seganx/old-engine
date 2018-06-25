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
            public AnimationClip clip = null;
        }

        public Animation target = null;
        public List<AnimationData> animations = new List<AnimationData>();
        private int lastPlayed = -1;

        public AnimationData PlayById(int id, bool once = true)
        {
            var res = animations.Find(x => x.id == id);
            if (!once || (res != null && lastPlayed != id))
            {
                lastPlayed = id;
                target.CrossFade(res.clip.name, 0.1f, PlayMode.StopAll);
            }
            return res;
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
                data.name = state.name;
                data.id = lastPlayed++;
                data.clip = state.clip;
                animations.Add(data);
            }
        }
#endif        
    }
}
