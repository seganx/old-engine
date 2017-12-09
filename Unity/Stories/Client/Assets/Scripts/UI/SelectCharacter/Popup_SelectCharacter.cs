using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;
using SelectCharacter;

public class Popup_SelectCharacter : GameState
{
    public ListItem prefab = null;
    public ScrollRect scroller = null;

    private System.Action<CharacterData> callbackFunc = null;

    public Popup_SelectCharacter Setup(CharacterData currentItem, System.Action<CharacterData> callback)
    {
        selected = currentItem;
        callbackFunc = callback;

        if (recentList == null) LoadRecentList();

        prefab.Clone<ListItem>(scroller.content).Setup(selected.isEmpty ? selected : CharacterData.Empty);
        prefab.Clone<ListItem>(scroller.content).Setup(selected.isPlayer ? selected : CharacterData.Player);

        foreach (var item in recentList)
            prefab.Clone<ListItem>(scroller.content).Setup(item, true);

        var assets = AssetCharacter.FindAll();
        foreach (var asset in assets)
        {
            prefab.Clone<ListItem>(scroller.content).Setup(new CharacterData()
            {
                family = asset.family,
                name = asset.name,
                body = asset.bodies[0].name,
                face = asset.faces[0].name,
                hair = asset.hairs[0].name
            });
        }

        return this;
    }

    public void OnConfirmed(bool isOk)
    {
        if (callbackFunc == null)
        {
            base.Back();
            return;
        }

        if (!isOk)
        {
            base.Back();
            callbackFunc(null);
        }
        else if (selected.isEmpty || selected.isPlayer)
        {
            base.Back();
            callbackFunc(selected);
        }
        else gameManager.OpenPopup<Popup_EditCharacter>().Setup(AssetCharacter.FindCharacter(selected.family), selected, cdata =>
            {
                if (cdata == null) return;

                // check to see if it's exist in recent list or not
                recentList.RemoveAll(x => x.name == cdata.name);

                //  item is not exist in recent list
                recentList.Insert(0, cdata);
                callbackFunc(cdata);
                base.Back();
            });
    }

    public override void Back()
    {
        OnConfirmed(false);
    }


    //////////////////////////////////////////////////////////
    // static members
    //////////////////////////////////////////////////////////
    public static CharacterData selected = null;
    public static List<CharacterData> recentList = new List<CharacterData>();


    public static void SaveRecentList()
    {
        PlayerPrefsEx.Serialize("SelectCharacter.RecentList", recentList);
    }

    public static void LoadRecentList()
    {
        var list = PlayerPrefsEx.Deserialize<List<CharacterData>>("SelectCharacter.RecentList");
        if (list != null && list.Count > 0)
            recentList = list;
    }
}
