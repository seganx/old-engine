using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;
using SelectDialog;

public class Popup_SelectDialog : GameState
{
    public ListItem prefab = null;
    public ScrollRect scroller = null;
    private System.Action<Book.Dialog> callbackFunc = null;

    public Popup_SelectDialog Setup(Book currentBook, Book.Dialog currentDialog, string selected, System.Action<Book.Dialog> callback)
    {
        callbackFunc = callback;

        //  inser null item
        {
            var nullItem = prefab.Clone<ListItem>(scroller.content).Setup(new Book.Dialog() { name = "none" });
            nullItem.characterFace.gameObject.SetActive(false);
            nullItem.OnSelect();
        }

        var dialogList = new List<Book.Dialog>(currentBook.dialogs);
        dialogList.Sort((x, y) => string.Compare(x.name, y.name));

        foreach (var item in dialogList)
        {
            if (item == currentDialog) continue;
            var listItem = prefab.Clone<ListItem>(scroller.content).Setup(item);
            if (listItem.data.name == selected)
                listItem.OnSelect();
        }

        return this;
    }

    public void OnConfirmed()
    {
        base.Back();
        if (callbackFunc == null) return;
        callbackFunc(selected);
    }

    public override void Back()
    {
        OnConfirmed();
    }

    //////////////////////////////////////////////////////////
    // static members
    //////////////////////////////////////////////////////////
    public static Book.Dialog selected = null;
}
