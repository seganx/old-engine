using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    [System.Serializable]
    public class ProfileData
    {
        public string nickname;
        public Value score;
        public Value xp;
        public Value level;

        public List<object> storage = new List<object>();
        public List<object> assets = new List<object>();
    }

}
