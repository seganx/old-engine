using UnityEngine;
using System.Collections;
using SeganX;

public class Weapon : Entity
{
    public enum DamageType { OnCollisionEnter, OnCollisionStay };

    public DamageType damageType = DamageType.OnCollisionEnter;

    public Weapon Setup(Json.Node root, Body body)
    {
        UpdateData(root);

        var basePath = "Machines/Weapon_" + name;
        switch (name)
        {
            case "Blade":
                Resources.Load<GameObject>(basePath).Clone<WeaponBlade>(transform).Setup(this);
                break;

            case "Chainsaw":
            case "Drill":
            case "Stinger":
                Resources.Load<GameObject>(basePath).Clone<WeaponSharp>(transform).Setup(this);
                break;

            case "Laser":
                Resources.Load<GameObject>(basePath).Clone<WeaponLaser>(transform).Setup(this);
                break;
        }
        SortSprites(gameObject, machine);

        if (body != null)
        {
            var id = root[Params.Slot].AsInt;
            var slot = body.weapons[id];
            transform.localPosition = slot.transform.localPosition;
        }

        if (machine.Party == MachineParty.Opponent)
            transform.localScale = new Vector3(-1, 1, 1);

        return this;
    }
}
