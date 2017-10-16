using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class UIMachinePart : MonoBehaviour
{
    public Text partName = null;
    public Image image = null;

    public UIMachinePart Setup(Json.Node node)
    {
        switch (node.Name)
        {
            case Params.Body: DisplayBody(node); break;
            case Params.Wheels: DisplayWheel(node); break;
            case Params.Staffs: DisplayStaff(node); break;
            case Params.Weapons: DisplayWeapon(node); break;
        }

        return this;
    }

    private void DisplayBody(Json.Node node)
    {

    }

    private void DisplayWheel(Json.Node node)
    {
        image.sprite = Entity.LoadSprite(Params.Wheel, name, node[Params.Material]);
    }

    private void DisplayStaff(Json.Node node)
    {

    }

    private void DisplayWeapon(Json.Node node)
    {

    }

}
