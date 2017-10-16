using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class UIPartList : MonoBehaviour
{
    public TextAsset jsonFile = null;

    public void Setup()
    {
        var root = Json.Parse(jsonFile.text);

        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
#if OFF
                case Params.Body:
                    foreach (var item in node.AsArray.Children)
                    {
                        
                    }
                    break;
                case Params.Wheels:
                    foreach (var item in node.AsArray.Children)
                    {
                        
                    }
                    break;
                case Params.Staffs:
                    foreach (var item in node.AsArray.Children)
                        Entity.Create<Staff>(transform).Setup(item);
                    break;
                case Params.Weapons:
                    foreach (var item in node.AsArray.Children)
                        Entity.Create<Weapon>(transform).Setup(item, null);
                    break;
#endif
            }
        }
    }

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}
