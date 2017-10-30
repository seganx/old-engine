using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public interface IProfileAsset { }

    [System.Serializable]
    public class ProfileData
    {
        public string nickname;
        public int score;
        public int xp;
        public int level;

        public List<IProfileAsset> storage = new List<IProfileAsset>();
        public List<IProfileAsset> assets = new List<IProfileAsset>();
    }

}
