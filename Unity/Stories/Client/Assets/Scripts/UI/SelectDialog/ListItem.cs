using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace SelectDialog
{
    public class ListItem : Base
    {
        public Image background = null;
        public Text nameLabel = null;
        public Image characterFace = null;
        public Text dialogText = null;
        public Book.Dialog data = null;

        public ListItem Setup(Book.Dialog dialog)
        {
            data = dialog;
            nameLabel.SetTextAndWrap(dialog.name);
            UpdateFace();
            dialogText.SetTextAndWrap(dialog.text, true);
            return this;
        }

        public void UpdateFace()
        {
            if (data.character.isEmpty)
                characterFace.sprite = gameManager.characterDefaults.Empty;
            else if (data.character.isPlayer)
                characterFace.sprite = gameManager.characterDefaults.Player;
            else
                characterFace.sprite = AssetCharacter.FindCharacterFace(data.character.family, data.character.face, gameManager.characterDefaults.Null);
        }

        public void OnSelect()
        {
            Popup_SelectDialog.selected = data;

            if (selected != null)
                selected.background.color = Color.white;
            selected = this;
            selected.background.color = Color.green;
        }

        //////////////////////////////////////////////////////////
        // static members
        //////////////////////////////////////////////////////////
        public static ListItem selected = null;
    }
}

