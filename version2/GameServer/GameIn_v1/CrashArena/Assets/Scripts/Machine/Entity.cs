using UnityEngine;
using System.Collections;
using SeganX;

public class Entity : MonoBehaviour
{
    public int grade = 0;
    public int material = 0;
    public int level = 1;
    public int power = 0;
    public float value = 5;
    public float mass = -1;
    public float health = 10;
    public float damage = 0;
    public float speed = 0;

    public IMachine machine = null;

    protected SpriteRenderer spriteRenderer = null;

    public float Height { get { return spriteRenderer ? spriteRenderer.sprite.rect.height / spriteRenderer.sprite.pixelsPerUnit : 0; } }

    public Entity UpdateData(Json.Node root)
    {
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case Params.Type: name = node.Value; break;
                case Params.Grade: grade = node.AsInt; break;
                case Params.Material: material = node.AsInt; break;
                case Params.Level: level = node.AsInt; break;
                case Params.Power: power = node.AsInt; break;
                case Params.Value: value = node.AsFloat; break;
                case Params.Mass: mass = node.AsFloat; break;
                case Params.Health: health = node.AsFloat; break;
                case Params.Damage: damage = node.AsFloat; break;
                case Params.Speed: speed = node.AsFloat; break;
            }
        }

        machine = GetComponentInParent<IMachine>();

        return this;
    }

    public Entity AddSpriteRenderer(string param)
    {
        spriteRenderer = gameObject.AddComponent<SpriteRenderer>();
        spriteRenderer.sortingLayerName = machine.SpriteLayerName;
        spriteRenderer.sortingOrder = machine.SpriteOrder++;
        spriteRenderer.sprite = LoadSprite(param, name, material);
        return this;
    }

    public Entity AddColliderWithRigidBody<T>() where T : Component
    {
        if (machine.IsEditMode) return this;

        if (GetComponent<T>() == null)
            gameObject.AddComponent<T>();

        var rb = GetComponent<Rigidbody2D>();
        if (rb == null)
            rb = gameObject.AddComponent<Rigidbody2D>();
        rb.mass = mass;

        return this;
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        var com = other.GetComponentInParent<Weapon>();
        if (com != null && com.damageType == Weapon.DamageType.OnCollisionEnter)
            machine.TakeDamage(com.damage);
    }

    void OnTriggerStay2D(Collider2D other)
    {
        var com = other.GetComponentInParent<Weapon>();
        if (com != null && com.damageType == Weapon.DamageType.OnCollisionStay)
            machine.TakeDamage(com.damage * com.speed * Time.deltaTime);
    }


    //////////////////////////////////////////////////////////////
    //  STATIC MEMBERS
    //////////////////////////////////////////////////////////////
    public static Sprite[] LoadSprites(string param, string name, int material)
    {
        var path = "Sprites/" + param + "/" + name;
        var res = Resources.LoadAll<Sprite>(path);
        if (res.IsNullOrEmpty())
            res = Resources.LoadAll<Sprite>(path + "/" + name + "_" + material);
        if (res.IsNullOrEmpty())
            res = new Sprite[10];
        return res;
    }

    public static Sprite LoadSprite(string param, string name, int material)
    {
        var allsprites = LoadSprites(param, name, material);
        var spriteName = name + "_" + material;

        Sprite res = null;
        foreach (var item in allsprites)
            if (item.name == spriteName)
                res = item;
        Resources.UnloadUnusedAssets();

        return res;
    }


    public static GameObject Flip(GameObject obj, MachineParty party)
    {
        if (party == MachineParty.Opponent)
        {
            var sprite = obj.GetComponent<SpriteRenderer>();
            if (sprite) sprite.flipX = true;

            var collider = obj.GetComponent<PolygonCollider2D>();
            if (collider != null)
            {
                var points = collider.points;
                for (int i = 0; i < points.Length; i++)
                    points[i].x *= -1;
                collider.points = points;
            }
        }
        return obj;
    }

    public static GameObject SortSprites(GameObject obj, IMachine machine)
    {
        var sprites = obj.GetComponentsInChildren<SpriteRenderer>(true);
        foreach (var item in sprites)
            item.sortingOrder = machine.SpriteOrder++;
        return obj;
    }

    public static T Create<T>(Transform parent) where T : Entity
    {
        var obj = new GameObject();
        var res = obj.AddComponent<T>();
        obj.transform.SetParent(parent, false);
        return res;
    }
}
