using UnityEngine;
using System.Collections;
using SeganX;

public class Staff : Entity
{

    public Staff Setup(Json.Node root)
    {
        UpdateData(root);

        return this;
    }

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
