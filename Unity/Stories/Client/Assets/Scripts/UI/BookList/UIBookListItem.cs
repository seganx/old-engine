using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;

public class UIBookListItem : Base
{
    public GameObject deleteButton = null;
    public GameObject addButton = null;
    public GameObject editButton = null;

    public UIBookListItem Setup()
    {
        Destroy(addButton);
        deleteButton.SetActive(true);
        editButton.SetActive(true);

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

    public void OnAddItem()
    {
        gameObject.Clone<UIBookListItem>().Setup().transform.SetAsFirstSibling();
       
    }

    public void OnDeleteItem()
    {
        Destroy(gameObject);
    }

    public void OnEditItem()
    {

    }
}
