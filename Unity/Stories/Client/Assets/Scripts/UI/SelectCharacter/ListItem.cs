using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

namespace SelectCharacter
{
    public class ListItem : GameState
    {
        public Image characterBack = null;
        public Character preview = null;
        public Text nameLabel = null;
        public GameObject removeButton = null;

        public CharacterData data = null;

        private Image background = null;

        public ListItem Setup(CharacterData charData, bool showRemoveButton = false)
        {
            data = charData;

            if (data.isEmpty) characterBack.sprite = gameManager.characterDefaults.Empty;
            else if (data.isPlayer) characterBack.sprite = gameManager.characterDefaults.Player;
            else preview.Setup(data);

            nameLabel.SetTextAndWrap(data.name);
            removeButton.SetActive(showRemoveButton);

            background = GetComponent<Image>();

            if (Popup_SelectCharacter.selected == data)
                OnSelect();

            return this;
        }

        public void OnRemove()
        {
            gameManager.OpenPopup<Popup_ConfirmBox>().Setup("Remove character " + data.name + " ?", true, yes =>
            {
                if (yes)
                {
                    Popup_SelectCharacter.recentList.Remove(data);
                    Destroy(gameObject);
                }
            });
        }

        public void OnSelect()
        {
            Popup_SelectCharacter.selected = data;

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
