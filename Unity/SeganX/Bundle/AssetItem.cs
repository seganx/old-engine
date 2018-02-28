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
            var ws = new WWW("locator.8khan.ir/Tests/Sajad/1.1/config.json");
            while (ws.isDone == false) ;
            Debug.Log(ws.text);
        }
    }
}
