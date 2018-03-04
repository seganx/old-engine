using UnityEngine;

namespace SeganX
{
    public class AssetItem : Base
    {
        [InspectorButton(100, "Generate Id", "GenerateId")]
        public int id = 0;
        public string type = "";
        [SpritePreview(50)]
        public Sprite preview = null;


        public void GenerateId(object x)
        {
            id = AssetIdGenerator.GenerateId();
        }
    }
}
