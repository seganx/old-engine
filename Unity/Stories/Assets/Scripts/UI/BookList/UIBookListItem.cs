using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class UIBookListItem : Base
{
    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }

    public void OnAddItem()
    {
        gameObject.Clone();
    }

    public void OnDeleteItem()
    {
        Destroy(gameObject);
    }

    public void OnEditItem()
    {

    }
}
