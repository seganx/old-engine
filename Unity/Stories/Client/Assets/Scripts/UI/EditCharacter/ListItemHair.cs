using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

namespace EditCharacter
{
    public class ListItemHair : Base
    {
        public Text nameLabel = null;
        public Image image = null;

        public Hair data = null;

        private Image background = null;
        private Popup_EditCharacter owner = null;

        public ListItemHair Setup(Hair hair)
        {
            data = hair;

            image.sprite = hair.preview;
            nameLabel.SetTextAndWrap(data.name);

            background = GetComponent<Image>();
            owner = GetComponentInParent<Popup_EditCharacter>();

            if (Popup_EditCharacter.currentSelected.hair == data.name)
                OnSelect();

            return this;
        }

        public void OnSelect()
        {
            Popup_EditCharacter.currentSelected.hair = data.name;
            owner.UpdatePreview();

            if (selected != null)
                selected.background.color = Color.white;
            selected = this;
            selected.background.color = Color.green;
        }

        //////////////////////////////////////////////////////////
        // static members
        //////////////////////////////////////////////////////////
        public static ListItemHair selected = null;
    }
}
