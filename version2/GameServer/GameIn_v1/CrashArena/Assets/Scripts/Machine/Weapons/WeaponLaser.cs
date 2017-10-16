using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponLaser : MonoBehaviour
{

    public SpriteRenderer body = null;
    public Animation laser = null;
    public Weapon parent = null;

    private float fightTime = 0;

    public WeaponLaser Setup(Weapon wparent)
    {
        parent = wparent;
        parent.damageType = Weapon.DamageType.OnCollisionStay;

        var sprites = Entity.LoadSprites(Params.Weapon, wparent.name, wparent.material);
        body.sprite = sprites[0];
        Resources.UnloadUnusedAssets();
        return this;
    }

    void Update()
    {
        if (parent.machine.IsEditMode) return;

        fightTime += Time.deltaTime / parent.speed;
        if (fightTime >= 1)
        {
            fightTime = 0;
            laser.Play();
        }
    }
}
