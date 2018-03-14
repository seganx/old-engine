using UnityEngine;
using UnityEngine.UI;

namespace SeganX
{
    public class LocalText : MonoBehaviour
    {
        public Text target = null;

        [TextArea(3, 100)]
        public string currnetText = "";

        public void SetFormatetText(object arg)
        {
            if (target) target.SetTextAndWrap(string.Format(currnetText, arg));
        }

        public void SetFormatetText(object arg0, object arg1)
        {
            if (target) target.SetTextAndWrap(string.Format(currnetText, arg0, arg1));
        }

        public void SetFormatetText(object arg0, object arg1, object arg2)
        {
            if (target) target.SetTextAndWrap(string.Format(currnetText, arg0, arg1, arg2));
        }

        public void SetFormatetText(object arg0, object arg1, object arg2, object arg3)
        {
            if (target) target.SetTextAndWrap(string.Format(currnetText, arg0, arg1, arg2, arg3));
        }

#if UNITY_EDITOR
        private void OnValidate()
        {
            if (target == null) target = transform.GetComponent<Text>(true, true);
            if (target) target.SetTextAndWrap(currnetText);
        }
#endif
    }
}
