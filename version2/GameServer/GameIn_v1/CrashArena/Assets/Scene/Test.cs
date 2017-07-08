using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class Test : MonoBehaviour
{
    public TextAsset jsonFile = null;

    public class Entity
    {
        public string sprite = "";
        public float mass = 1;
    }

    public class Wheel: Entity
    {
        public Vector2 pos = Vector2.zero;
        public float speed = 30;
    }


    public class Machine
    {
        public string name = "";
        public Entity body = new Entity();
        public List<Wheel> wheels = new List<Wheel>();
    }

    // Use this for initialization
    IEnumerator Start()
    {
        yield return new WaitForSeconds(1);

        var root = JSON.Parse(jsonFile.text);
        Debug.Log(root.ToString());
        HandleJsonObject(root);
    }

    void HandleJsonObject(JSONNode root)
    {
        var machine = new Machine();
        
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case "name": machine = new Machine() { name = node.Value }; break;
                case "body": machine.body = HandleJsonEntity(machine.body, node); break;
                case "wheel": machine.wheels.Add(HandleJsonWheel(node)); break;
                default: 
                    Debug.Log(node.Name + ":" + node.Value);
                    HandleJsonObject(node); 
                    break;
            }
        }

        Debug.Log(machine);
    }

    Vector4 HandleJsonVector(JSONNode root)
    {
        Vector4 res = Vector4.zero;
        for (int i = 0; i < root.Count && i < 4; i++)
            res[i] = root[i].AsFloat;
        return res;
    }

    Entity HandleJsonEntity(Entity obj, JSONNode root)
    {
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case "sprite": obj.sprite = node.Value; break;
                case "mass": obj.mass = node.AsFloat; break;
            }
        }
        return obj;
    }

    Wheel HandleJsonWheel(JSONNode root)
    {
        var res = new Wheel();
        HandleJsonEntity(res, root);
        for (int i = 0; i < root.Count; i++)
        {
            var node = root[i];
            switch (node.Name)
            {
                case "pos": res.pos = HandleJsonVector(node); break;
                case "speed": res.speed = node.AsFloat; break;
            }
        }
        return res;
    }

    // Update is called once per frame
    void Update()
    {

    }
}
