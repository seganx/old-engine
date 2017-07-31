using UnityEngine;
using System.Collections;

public class Staff : Entity
{

    public Staff Setup(JSONNode root)
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
