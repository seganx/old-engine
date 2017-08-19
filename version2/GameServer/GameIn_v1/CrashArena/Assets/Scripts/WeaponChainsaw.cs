using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponChainsaw : MonoBehaviour {

    public SpriteRenderer body = null;
    public Transform saw = null;
    public Weapon parent = null;

    private int imageIndex = 0;

    public WeaponChainsaw Setup(Weapon wparent)
    {
        parent = wparent;
        parent.damageType = Weapon.DamageType.OnCollisionStay;

        var sprites = wparent.LoadSprites(Params.Weapon);
        body.sprite = sprites[0];
        Resources.UnloadUnusedAssets();
        return this;
    }

    void Update()
    {
        saw.SetActiveChild(imageIndex++ % 3);
    }
}
