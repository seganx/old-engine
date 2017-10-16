using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SeganX;


public class Machine : MonoBehaviour, IMachine
{
    public float totalHealth = 0;
    public float totalDamage = 0;

    public Body body = null;
    public List<Wheel> wheels = new List<Wheel>();
    public List<Staff> staffs = new List<Staff>();
    public List<Weapon> weapons = new List<Weapon>();

    public MachineParty Party { get; set; }
    public int SpriteOrder { get; set; }
    public bool IsEditMode{ get { return Party == MachineParty.Editor; } }
    public string SpriteLayerName { get { return "Objects"; } }

    public Machine Setup(Json.Node root, MachineParty machineParty)
    {
        Party = machineParty;

        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case Params.Name: name = node.Value; break;
                case Params.Body: body = Entity.Create<Body>(transform).Setup(node); break;
                case Params.Wheels:
                    foreach (var item in node.AsArray.Children)
                        wheels.Add(Entity.Create<Wheel>(transform).Setup(item, body));
                    break;
                case Params.Staffs:
                    foreach (var item in node.AsArray.Children)
                        staffs.Add(Entity.Create<Staff>(transform).Setup(item));
                    break;
                case Params.Weapons:
                    foreach (var item in node.AsArray.Children)
                        weapons.Add(Entity.Create<Weapon>(body.transform).Setup(item, body));
                    break;
            }
        }

        return this;
    }

    public void TakeDamage(float damage)
    {
        totalHealth -= damage;
        if (totalHealth <= 0)
        {
            Debug.Log(name + ": Destroyed!");
            Destroy(gameObject);
        }
    }

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }


    //////////////////////////////////////////////////////////////
    //  STATIC MEMBERS
    //////////////////////////////////////////////////////////////
    public static Machine Create()
    {
        var obj = new GameObject();
        return obj.AddComponent<Machine>();
    }
}
