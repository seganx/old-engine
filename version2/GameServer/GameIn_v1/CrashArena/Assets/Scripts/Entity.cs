using UnityEngine;
using System.Collections;


public class Entity : MonoBehaviour
{
    public int grade = 0;
    public int rarity = 0;
    public int level = 1;
    public int power = 0;
    public float value = 5;
    public float mass = -1;
    public float health = 10;
    public float damage = 0;
    public float speed = 0;
    public Machine machine = null;

    public Entity UpdateData(JSONNode root)
    {
        machine = GetComponentInParent<Machine>();

        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case Params.Type: name = node.Value; break;
                case Params.Grade: grade = node.AsInt; break;
                case Params.Rarity: rarity = node.AsInt; break;
                case Params.Level: level = node.AsInt; break;
                case Params.Power: power = node.AsInt; break;
                case Params.Value: value = node.AsFloat; break;
                case Params.Mass: mass = node.AsFloat; break;
                case Params.Health: health = node.AsFloat; break;
                case Params.Damage: damage = node.AsFloat; break;
                case Params.Speed: speed = node.AsFloat; break;
            }
        }

        return this;
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        var com = other.GetComponentInParent<Entity>();
        if (com != null)
        {
            health -= com.damage;
            if (health <= 0)
            {
                Debug.Log(name + ": Destroyed!");
                Destroy(gameObject);
            }
        }
    }

    public Sprite[] LoadSprites(string param)
    {
        var path = "Sprites/" + param + "/" + name;
        var res = Resources.LoadAll<Sprite>(path);
        if (res.IsNullOrEmpty())
            res = Resources.LoadAll<Sprite>(path + "_" + grade);
        return res;
    }

    public SpriteRenderer CheckSprite(string param)
    {
        var sprite = GetComponent<SpriteRenderer>();
        if (sprite == null) return null;

        var allsprites = LoadSprites(param);
        var spriteName = name + "_" + grade;
        foreach (var item in allsprites)
            if (item.name == spriteName)
                sprite.sprite = item;

        Resources.UnloadUnusedAssets();
        return sprite;
    }

    public Entity CheckRigidbody()
    {
        if (mass > 0 && machine.side != Side.Null)
            gameObject.AddComponent<Rigidbody2D>().mass = mass;
        return this;
    }

    public static GameObject Flip(GameObject obj, Side side)
    {
        if (side == Side.Opponent)
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
}
