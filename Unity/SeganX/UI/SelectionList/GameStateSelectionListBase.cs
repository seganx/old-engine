using UnityEngine;
using SeganX;

public class GameStateSelectionListBase<T> : GameState
{
    public Color itemColorDefault = Color.white;
    public Color itemColorSelected = Color.green;

    public SelectionItemBase<T> selected = null;
}
