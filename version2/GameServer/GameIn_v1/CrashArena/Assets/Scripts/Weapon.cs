using UnityEngine;
using System.Collections;
using SeganX;

public class Weapon : Entity
{
    public enum DamageType { OnCollisionEnter, OnCollisionStay };

    public DamageType damageType = DamageType.OnCollisionEnter;

    public Weapon Setup(Json.Node root)
    {
        UpdateData(root);

        var id = root[Params.Slot].AsInt;
        var slot = machine.body.weapons[id];
        transform.localPosition = slot.transform.localPosition;

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

        if (machine.side == Side.Opponent)
            transform.localScale = new Vector3(-1, 1, 1);

        return this;
    }
}
