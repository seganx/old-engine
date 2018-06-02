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

        private string displayText = "";

        public void SetText(string text)
        {
            if (currnetText == text) return;
            displayText = currnetText = text;
            DisplayText();
        }

        public void SetFormatetText(object arg)
        {
            displayText = string.Format(currnetText, arg);
            DisplayText();
        }

        public void SetFormatetText(object arg0, object arg1)
        {
            displayText = string.Format(currnetText, arg0, arg1);
            DisplayText();
        }

        public void SetFormatetText(object arg0, object arg1, object arg2)
        {
            displayText = string.Format(currnetText, arg0, arg1, arg2);
            DisplayText();
        }

        public void SetFormatetText(object arg0, object arg1, object arg2, object arg3)
        {
            displayText = string.Format(currnetText, arg0, arg1, arg2, arg3);
            DisplayText();
        }

        public void Awake()
        {
            if (stringId > 0)
                SetText(LocalizationService.Get(stringId));
            else
            {
                displayText = currnetText;
                DisplayText();
            }
        }

        private void DisplayText()
        {
            target.SetTextAndWrap(displayText, autoRtl, forcePersian);
        }

#if UNITY_EDITOR
        private string lastText = "";
        private void OnValidate()
        {
            if (UnityEditor.EditorApplication.isPlaying == false)
            {
                if (target == null)
                    target = transform.GetComponent<Text>(true, true);

                if (lastText != currnetText)
                {
                    lastText = displayText = currnetText;
                    DisplayText();
                }
            }
        }

        private void OnRectTransformDimensionsChange()
        {
            if (UnityEditor.EditorApplication.isPlaying == false)
                DisplayText();
        }
#endif

    }
}
