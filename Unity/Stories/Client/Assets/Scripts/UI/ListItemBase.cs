using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class ListItemBase : Base
{
    public Image background = null;

    public object data = null;

    public virtual ListItemBase Setup(object dataObject)
    {
        data = dataObject;
        return this;
    }

    public virtual void OnSelect()
    {
        if (selected != null)
            selected.background.color = Color.white;
        selected = this;
        selected.background.color = Color.green;
    }

    //////////////////////////////////////////////////////////
    // static members
    //////////////////////////////////////////////////////////
    public static ListItemBase selected = null;
}
