using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

namespace SeganX
{
    [ExecuteInEditMode]
    [RequireComponent(typeof(Text))]
    [AddComponentMenu("UI/SeganX/LocalText")]
    public class LocalText : Base
    {
        public Text target = null;
        public bool autoRtl = false;
        public bool forcePersian = false;
        public int stringId = 0;

        [TextArea(3, 100)]
        public string currnetText = "";

        public void SetText(string text)
        {
            if (currnetText == text) return;
            currnetText = text;
            target.SetTextAndWrap(currnetText, autoRtl, forcePersian);
        }

        public void SetFormatetText(object arg)
        {
            target.SetTextAndWrap(string.Format(currnetText, arg), autoRtl, forcePersian);
        }

        public void SetFormatetText(object arg0, object arg1)
        {
            target.SetTextAndWrap(string.Format(currnetText, arg0, arg1), autoRtl, forcePersian);
        }

        public void SetFormatetText(object arg0, object arg1, object arg2)
        {
            target.SetTextAndWrap(string.Format(currnetText, arg0, arg1, arg2), autoRtl, forcePersian);
        }

        public void SetFormatetText(object arg0, object arg1, object arg2, object arg3)
        {
            target.SetTextAndWrap(string.Format(currnetText, arg0, arg1, arg2, arg3), autoRtl, forcePersian);
        }

        public void Awake()
        {
            if (stringId > 0)
            {
                currnetText = LocalizationService.Get(stringId);
                target.SetTextAndWrap(currnetText, autoRtl, forcePersian);
            }
        }

#if UNITY_EDITOR
        private void OnValidate()
        {
            if (target == null) target = transform.GetComponent<Text>(true, true);
            target.SetTextAndWrap(currnetText, autoRtl, forcePersian);
        }

        private void OnRectTransformDimensionsChange()
        {
            if (UnityEditor.EditorApplication.isPlaying == false)
                target.SetTextAndWrap(currnetText, autoRtl, forcePersian);
        }
#endif

    }
}
