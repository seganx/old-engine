using UnityEngine;
using System.Collections;

public class Gesture_Zoom : Gesture_Base
{
    //  gestures will be sorted by priorities
    public override GestureType Type
    {
        get { return GestureType.Zoom; }
    }


}
