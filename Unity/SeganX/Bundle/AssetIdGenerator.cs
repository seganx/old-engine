using UnityEngine;


namespace SeganX
{
    public static class AssetIdGenerator
    {
        [System.Serializable]
        private class ServerId
        {
            public int id = 0;
        }

#if UNITY_EDITOR
#else
        private static int baseId = 10;
#endif

        public static int GenerateId()
        {
#if UNITY_EDITOR
            var ws = new WWW("http://172.16.11.216:8060/");
            while (ws.isDone == false) ;
            var sid = JsonUtility.FromJson<ServerId>(ws.text);
            return sid.id;
#else
            return baseId++;
#endif
        }

    }
}
