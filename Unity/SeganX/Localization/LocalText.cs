using UnityEngine;
using UnityEngine.UI;

namespace SeganX
{
    [ExecuteInEditMode]
    [RequireComponent(typeof(Text))]
    public class LocalText : Base
    {
        [System.Serializable]
        public class LocalKit
        {
            public int id = 0;
            public Localization localization = null;
            [InspectorButton(100, "Save Text", "OnSaveText", true)]
            public int buttons = 0;
        }
        
        public Text target = null;

        [TextArea(3, 100)]
        public string currnetText = "";
        public bool autoRtl = false;
        public bool forcePersian = false;

        public LocalKit localKit;

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

#if UNITY_EDITOR
        public void LoadTextFromlocalization()
        {
            if (localKit.id > 0)
            {
                currnetText = localKit.localization.Get(localKit.id);
                target.SetTextAndWrap(currnetText);
            }
        }

        public void OnSaveText(object sender)
        {
            if (localKit.localization)
            {
                localKit.id = localKit.localization.UpdateString(localKit.id, currnetText);
                localKit.localization.Save();
            }
        }

        private void OnValidate()
        {
            if (target == null) target = transform.GetComponent<Text>(true, true);



            target.SetTextAndWrap(currnetText);
        }

        private void OnRectTransformDimensionsChange()
        {
            if (UnityEditor.EditorApplication.isPlaying == false)
                target.SetTextAndWrap(currnetText);
        }
#endif

    }
}
