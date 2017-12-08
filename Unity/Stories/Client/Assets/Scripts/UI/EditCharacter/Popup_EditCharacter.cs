using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;
using EditCharacter;

public class Popup_EditCharacter : GameState
{
    public Character preview = null;

    public ScrollRect hairScroller = null;
    public ScrollRect faceScroller = null;
    public ScrollRect bodyScroller = null;

    public ListItemBody bodyItemPrefab = null;
    public ListItemHair hairItemPrefab = null;
    public ListItemFace faceItemPrefab = null;

    private System.Action<CharacterData> callbackFunc = null;

    public Popup_EditCharacter Setup(AssetCharacter source, CharacterData currentData, System.Action<CharacterData> callback)
    {
        selected = currentData ?? source.Default;
        callbackFunc = callback;
        UpdatePreview();

        foreach (var item in source.hairs)
            hairItemPrefab.Clone<ListItemHair>(hairScroller.content).Setup(item);

        foreach (var item in source.faces)
            faceItemPrefab.Clone<ListItemFace>(faceScroller.content).Setup(item);

        foreach (var item in source.bodies)
            bodyItemPrefab.Clone<ListItemBody>(bodyScroller.content).Setup(item);

        return this;
    }

    public void UpdatePreview()
    {
        preview.Setup(selected);
    }
    
    public void OnConfirmed(bool isOk)
    {
        base.Back();

        if (callbackFunc != null)
            callbackFunc(isOk ? selected : null);
    }

    public override void Back()
    {
        OnConfirmed(false);
    }


    //////////////////////////////////////////////////////////////////////////////////
    //  static members
    //////////////////////////////////////////////////////////////////////////////////
    public static CharacterData selected = null;

}
