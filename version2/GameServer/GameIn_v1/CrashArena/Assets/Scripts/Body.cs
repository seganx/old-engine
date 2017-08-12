using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SeganX;

public class Body : Entity
{
    public List<Slot> wheels = new List<Slot>();
    public List<Slot> staffs = new List<Slot>();
    public List<Slot> weapons = new List<Slot>();

    public Body Setup(Json.Node root)
    {
        UpdateData(root);
        CheckSprite(Params.Body);

        foreach (var item in root[Params.Slots].AsArray.Children)
        {
            var slot = Resources.Load("Machines/Slot").Clone<Slot>(transform).Setup(this, item);
            switch (slot.type)
            {
                case Params.Wheel: wheels.Add(slot); break;
                case Params.Staff: staffs.Add(slot); break;
                case Params.Weapon: weapons.Add(slot); break;
            }
        }

        gameObject.AddComponent<PolygonCollider2D>();
        Flip(gameObject, machine.side);
        CheckRigidbody();

        return this;
    }

    void OnDestroy()
    {
        Destroy(machine.gameObject);
    }
}
