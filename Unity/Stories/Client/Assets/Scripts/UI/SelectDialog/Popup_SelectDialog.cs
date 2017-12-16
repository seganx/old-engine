using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class Popup_SelectDialog : GameStateSelectBase
{
    public ListItemBase nonePrefab = null;

    public Popup_SelectDialog Setup(Book currentBook, Book.Dialog currentDialog, string selected, System.Action<object> callback)
    {
        callbackFunc = callback;

        //  inser null item
        nonePrefab.Clone<ListItemBase>(scroller.content).Setup(new Book.Dialog() { name = "none" }).OnSelect();

        var dialogList = new List<Book.Dialog>(currentBook.dialogs);
        dialogList.Sort((x, y) => string.Compare(x.name, y.name));

        foreach (var item in dialogList)
        {
            if (item == currentDialog) continue;

            var listItem = prefab.Clone<ListItemDialog>(scroller.content).Setup(item);
            if (listItem.data.As<Book.Dialog>().name == selected)
                listItem.OnSelect();
        }

        return this;
    }
}
