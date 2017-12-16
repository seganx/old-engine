using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class GameStateSelectBase : GameState
{
    public ListItemBase prefab = null;
    public ScrollRect scroller = null;

    protected System.Action<object> callbackFunc = null;

    public void OnConfirmed(bool isOk)
    {
        base.Back();
        if (callbackFunc == null) return;
        callbackFunc(isOk ? ListItemBase.selected.data : null);
    }

    public override void Back()
    {
        OnConfirmed(false);
    }
}
