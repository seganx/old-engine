using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;


public class Popup_InputText : GameState
{
    public InputField inputField = null;

    private System.Action<string> callbackFunc = null;

    public Popup_InputText Setup(string text, string placeHolder, System.Action<string> callback)
    {
        callbackFunc = callback;
        inputField.text = text;
        if (placeHolder.HasContent())
            inputField.placeholder.GetComponent<Text>().text = placeHolder;
        return this;
    }

    public void OnConfirm(bool isOk)
    {
        if (callbackFunc != null)
            callbackFunc(isOk ? inputField.text.Replace('ي', 'ی').Replace("‌", "") : null);
        base.Back();
    }

    public override void Back()
    {
        OnConfirm(false);
    }
}
