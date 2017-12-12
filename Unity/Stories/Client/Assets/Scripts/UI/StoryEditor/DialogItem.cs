using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.UI.Extensions;
using UnityEngine;

namespace StoryEditor
{
    public class DialogItem : Base
    {
        public Image characterFace = null;
        public Text dialogText = null;
        public DialogItem[] links = new DialogItem[2] { null, null };
        public UILineRenderer[] lines = new UILineRenderer[2] { null, null };


        public Book.Dialog data = null;

        public DialogItem Setup(Book.Dialog dialog)
        {
            data = dialog;
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

        public void UpdateLinksVisuals()
        {
            for (int i = 0; i < lines.Length; i++)
            {
                if (links[i] != null)
                {
                    var distance = links[i].rectTransform.anchoredPosition - rectTransform.anchoredPosition;
                    if (Mathf.Abs(distance.x) > 200 || Mathf.Abs(distance.y) > 400)
                    {
                        var dest = links[i].rectTransform.anchoredPosition3D - (rectTransform.anchoredPosition3D + lines[i].rectTransform.anchoredPosition3D);

                        var points = lines[i].Points;
                        points[1] = dest;
                        lines[i].Points = points;
                        lines[i].enabled = true;
                    }
                    else lines[i].enabled = false;
                }
                else lines[i].enabled = false;
            }
        }

        public void OnEditText()
        {
            gameManager.OpenPopup<Popup_DialogEditor>().Setup(data, (isOk) =>
            {
                if (isOk) Setup(data);
            });
        }

        public void OnEditCharacter()
        {
            gameManager.OpenPopup<Popup_SelectCharacter>().Setup(data.character, cdata =>
            {
                if (cdata != null)
                {
                    data.character = cdata;
                    UpdateFace();
                }
            });
        }
    }

}
