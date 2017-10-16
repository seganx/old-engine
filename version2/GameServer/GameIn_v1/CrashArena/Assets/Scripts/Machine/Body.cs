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
        AddSpriteRenderer(Params.Body);
        AddColliderWithRigidBody<PolygonCollider2D>();
        Flip(gameObject, machine.Party);

        AddSlots(root[Params.Slots].AsArray);

        LoadAdditionalVisual();

        return this;
    }

    private void AddSlots(Json.Array jarray)
    {
        foreach (var item in jarray.Children)
        {
            var slot = Slot.Create(transform).Setup(this, item);
            switch (slot.type)
            {
                case Params.Wheel: wheels.Add(slot); break;
                case Params.Staff: staffs.Add(slot); break;
                case Params.Weapon: weapons.Add(slot); break;
            }

            SortSprites(slot.gameObject, machine);
        }
    }

    private void LoadAdditionalVisual()
    {
        var additional = Resources.Load("Machines/Body_" + material);
        if (additional != null)
        {
            var child = additional.Clone<Transform>(transform);
            child.localPosition = Vector3.zero;
            if (machine.Party == MachineParty.Opponent)
                child.localScale = new Vector3(-1, 1, 1);
            else
                child.localScale = Vector3.one;
        }
    }
}
