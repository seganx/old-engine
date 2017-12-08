using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

namespace EditCharacter
{
    public class ListItemFace : Base
    {
        public Text nameLabel = null;
        public RectTransform holder = null;

        public Face data = null;

        private Image background = null;
        private Popup_EditCharacter owner = null;

        public ListItemFace Setup(Face face)
        {
            data = face;

            face.Clone<Face>(holder);
            nameLabel.SetTextAndWrap(data.name);

            background = GetComponent<Image>();
            owner = GetComponentInParent<Popup_EditCharacter>();

            if (Popup_EditCharacter.selected.face == data.name)
                OnSelect();

            return this;
        }

        public void OnSelect()
        {
            Popup_EditCharacter.selected.face = data.name;
            owner.UpdatePreview();

            if (selected != null)
                selected.background.color = Color.white;
            selected = this;
            selected.background.color = Color.green;
        }

        //////////////////////////////////////////////////////////
        // static members
        //////////////////////////////////////////////////////////
        public static ListItemFace selected = null;
    }
}
