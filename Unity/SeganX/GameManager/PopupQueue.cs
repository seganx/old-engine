using System.Collections;
using System.Collections.Generic;

namespace SeganX
{
    public class PopupQueue : Base
    {
        private static List<System.Action> commands = new List<System.Action>(20);

        public static void Add(System.Action callback)
        {
            commands.Add(callback);
        }

        // Update is called once per frame
        void Update()
        {
            if (gameManager.CurrentPopup != null || commands.Count < 1) return;
            var curr = commands[0];
            commands.RemoveAt(0);
            curr();
        }
    }
}
