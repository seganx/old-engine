using UnityEngine;

namespace SeganX
{
    public class GUIPage : GUIPanel
    {
        //  return the time of closing animation
        public virtual float Close()
        {
            return 0;
        }

        public override void Back()
        {
            guiManager.ClosePage();
        }
    }
}