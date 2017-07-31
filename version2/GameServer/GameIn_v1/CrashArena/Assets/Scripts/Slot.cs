using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class Slot : MonoBehaviour
{
    public string type = "";
    public float frequency = 10;

    public Slot Setup(Body body, JSONNode root)
    {
        Vector2 pos = Vector2.zero;
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case Params.Type: type = node.Value; break;
                case Params.X: pos.x = node.AsFloat; break;
                case Params.Y: pos.y = node.AsFloat; break;
                case Params.Frequency: frequency = node.AsFloat; break;
            }
        }

        if (body.machine.side == Side.Opponent)
            pos.x *= -1;
        transform.localPosition = pos;

        switch (type)
        {
            case Params.Wheel: transform.RemoveChildrenBut(0); break;
            case Params.Staff: transform.RemoveChildrenBut(1); break;
            case Params.Weapon: transform.RemoveChildrenBut(2); break;
        }

        Entity.Flip(transform.GetChild(0).gameObject, body.machine.side);
        return this;
    }

    public void Click()
    {

    }
}

