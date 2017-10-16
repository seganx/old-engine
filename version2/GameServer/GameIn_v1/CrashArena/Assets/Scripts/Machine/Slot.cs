using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SeganX;


public class Slot : MonoBehaviour
{
    public string type = "";
    public float frequency = 10;

    public Slot Setup(Body body, Json.Node root)
    {
        Vector2 pos = Vector2.zero;
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case Params.Type: name = type = node.Value; break;
                case Params.X: pos.x = node.AsFloat; break;
                case Params.Y: pos.y = node.AsFloat; break;
                case Params.Frequency: frequency = node.AsFloat; break;
            }
        }

        if (body.machine.Party == MachineParty.Opponent)
            pos.x *= -1;
        transform.localPosition = pos;

        var sprite = gameObject.AddComponent<SpriteRenderer>();
        sprite.sortingLayerName = "Objects";
        sprite.sortingOrder = body.machine.SpriteOrder++;
        switch (type)
        {
            case Params.Wheel: sprite.sprite = Entity.LoadSprite(Params.Body, "Slot", 0); break;
            case Params.Staff: sprite.sprite = Entity.LoadSprite(Params.Body, "Slot", 1); break;
            case Params.Weapon: sprite.sprite = Entity.LoadSprite(Params.Body, "Slot", 2); break;
        }
        Entity.Flip(gameObject, body.machine.Party);

        return this;
    }


    //////////////////////////////////////////////////////////////
    //  STATIC MEMBERS
    //////////////////////////////////////////////////////////////
    public static Slot Create(Transform parent)
    {
        var obj = new GameObject("Slot");
        obj.transform.SetParent(parent, false);
        return obj.AddComponent<Slot>();
    }
}

