using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;
using EditCharacter;

public class Popup_EditCharacter : GameState
{
    public Character preview = null;
    public InputField characterName = null;

    public ScrollRect hairScroller = null;
    public ScrollRect faceScroller = null;
    public ScrollRect bodyScroller = null;

    public ListItemBody bodyItemPrefab = null;
    public ListItemHair hairItemPrefab = null;
    public ListItemFace faceItemPrefab = null;

    private System.Action<CharacterData> callbackFunc = null;

    public Popup_EditCharacter Setup(AssetCharacter source, CharacterData currentData, System.Action<CharacterData> callback)
    {
        callbackFunc = callback;

        currentSelected = currentData != null ? currentData.Clone() : source.Default;

        UpdatePreview();
        characterName.text = currentSelected.name;

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
        preview.Setup(currentSelected);
    }

    public void OnConfirmed(bool isOk)
    {
        if (isOk == false)
        {
            callbackFunc(null);
            base.Back();
            return;
        }

        if (characterName.text.HasContent(3) == false)
        {
            characterName.targetGraphic.color = Color.yellow;
            return;
        }
        else base.Back();

        currentSelected.name = characterName.text;
        callbackFunc(currentSelected);
    }

    public override void Back()
    {
        OnConfirmed(false);
    }


    //////////////////////////////////////////////////////////////////////////////////
    //  static members
    //////////////////////////////////////////////////////////////////////////////////
    public static CharacterData currentSelected = new CharacterData();

}
