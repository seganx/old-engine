using UnityEngine;
using UnityEngine.UI;

namespace SeganX
{
    public class LocalText : MonoBehaviour
    {
        public Text target = null;

#if UNITY_EDITOR
        [TextArea(3, 100)]
        public string currnetText = "";

        private void OnValidate()
        {
            if (target == null)
                target = transform.GetComponent<Text>(true, true);

            if (target)
                target.SetTextAndWrap(currnetText);
        }
#endif
    }
}
