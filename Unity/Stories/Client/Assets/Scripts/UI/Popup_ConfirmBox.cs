using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class Popup_ConfirmBox : GameState
{
    public Text desc = null;
    public GameObject cancelButton = null;

    private System.Action<bool> callback = null;

    public Popup_ConfirmBox Setup(string text, bool displayCancelButton, System.Action<bool> onOkCallback)
    {
        callback = onOkCallback;
        cancelButton.SetActive(displayCancelButton);
        desc.SetTextAndWrap(text);
        return this;
    }
    
    public void OnConfirm(bool value)
    {
        base.Back();

        if (callback != null)
            callback(value);
    }

    public override void Back()
    {
        OnConfirm(false);
    }
}

