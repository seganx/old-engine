using UnityEngine;
using System.Collections;

public class ColliderCallback : MonoBehaviour
{
    public int health = 10;
    public int damage = 10;
    public SpriteRenderer sprite = null;

    // Use this for initialization
    void Start()
    {
        if (sprite == null)
            sprite = GetComponent<SpriteRenderer>();
    }

    // Update is called once per frame
    void Update()
    {
        if (health < 1)
            Destroy(gameObject);
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        var com = other.GetComponent<ColliderCallback>();
        if (com != null)
        {
            sprite.color = Color.red;
            health -= com.damage;
        }
    }

    void OnTriggerExit2D(Collider2D other)
    {
        sprite.color = Color.white;
    }
}
