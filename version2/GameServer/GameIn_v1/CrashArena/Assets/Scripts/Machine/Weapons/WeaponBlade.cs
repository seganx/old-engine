using UnityEngine;
using System.Collections;

public class WeaponBlade : MonoBehaviour
{
    public SpriteRenderer handle = null;
    public SpriteRenderer arm = null;
    public SpriteRenderer saw = null;
    public SpriteRenderer head = null;
    public Weapon parent = null;

    public WeaponBlade Setup(Weapon wparent)
    {
        parent = wparent;
        parent.damageType = Weapon.DamageType.OnCollisionEnter;

        var sprites = Entity.LoadSprites(Params.Weapon, wparent.name, wparent.material);
        saw.sprite = sprites[0];
        handle.sprite = sprites[1];
        arm.sprite = sprites[2];
        head.sprite = sprites[3];
        Resources.UnloadUnusedAssets();

        saw.gameObject.AddComponent<CircleCollider2D>().isTrigger = true;
        GetComponent<Rotator>().speed = wparent.speed;
        return this;
    }
}
