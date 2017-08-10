using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public enum Side { Null, Player, Opponent }

public class Machine : MonoBehaviour
{
    public float totalHealth = 0;
    public float totalDamage = 0;

    public Side side = Side.Null;
    public Body body = null;
    public List<Wheel> wheels = new List<Wheel>();
    public List<Staff> staffs = new List<Staff>();
    public List<Weapon> weapons = new List<Weapon>();

    public Machine Setup(JSONNode root, Side playerSide)
    {
        side = playerSide;

        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case "name": name = node.Value; break;
                case "body": body = Resources.Load("Machines/Body").Clone<Body>(transform).Setup(node); break;
                case "wheels":
                    foreach (var item in node.AsArray.Children)
                        wheels.Add(Resources.Load("Machines/Wheel").Clone<Wheel>(transform).Setup(item));
                    break;
                case "staffs":
                    foreach (var item in node.AsArray.Children)
                        staffs.Add(Resources.Load("Machines/Staff").Clone<Staff>(transform).Setup(item));
                    break;
                case "weapons":
                    foreach (var item in node.AsArray.Children)
                        weapons.Add(Resources.Load("Machines/Weapon").Clone<Weapon>(body.transform).Setup(item));
                    break;
            }
        }
        return this;
    }

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}
