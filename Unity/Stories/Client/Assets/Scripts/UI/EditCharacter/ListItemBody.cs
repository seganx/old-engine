using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

namespace EditCharacter
{
    public class ListItemBody : Base
    {
        public Text nameLabel = null;
        public Image image = null;

        public Body data = null;

        private Image background = null;
        private Popup_EditCharacter owner = null;

        public ListItemBody Setup(Body body)
        {
            data = body;

            image.sprite = body.preview;
            nameLabel.SetTextAndWrap(data.name);

            background = GetComponent<Image>();
            owner = GetComponentInParent<Popup_EditCharacter>();

            if (Popup_EditCharacter.currentSelected.body == data.name)
                OnSelect();

            return this;
        }

        public void OnSelect()
        {
            Popup_EditCharacter.currentSelected.body = data.name;
            owner.UpdatePreview();

            if (selected != null)
                selected.background.color = Color.white;
            selected = this;
            selected.background.color = Color.green;
        }

        //////////////////////////////////////////////////////////
        // static members
        //////////////////////////////////////////////////////////
        public static ListItemBody selected = null;
    }
}
