using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SeganX;
using StoryEditor;

public class State_StoryEditor : GameState
{
    public MainPanel mainPanel = null;
    public EditorPanel editorPanel = null;

    public void OnZoom(int index)
    {
        switch (index)
        {
            case -1: editorPanel.scroller.content.Scale(0.8f, 0.8f, 1); break;
            case 0: editorPanel.scroller.content.localScale = Vector3.one; break;
            case +1: editorPanel.scroller.content.Scale(1.2f, 1.2f, 1); break;
        }
    }

    public override void Back()
    {
        //base.Back();
    }
}
