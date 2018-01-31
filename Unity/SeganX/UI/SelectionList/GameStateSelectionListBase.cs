using UnityEngine;
using SeganX;

public class GameStateSelectionListBase<T> : GameState
{
    public Color itemColorDefault = Color.white;
    public Color itemColorSelected = Color.green;
    public SelectionItemBase<T> selected = null;

    protected System.Action<bool, T> callbackFunc;

    public GameStateSelectionListBase<T> SetupCallback(System.Action<bool, T> callback)
    {
        callbackFunc -= callback;
        callbackFunc += callback;
        return this;
    }

    public void OnConfirm(bool isOk)
    {
        callbackFunc(isOk, selected ? selected.data : default(T));
        base.Back();
    }

    public override void Back()
    {
        OnConfirm(false);
    }
}
