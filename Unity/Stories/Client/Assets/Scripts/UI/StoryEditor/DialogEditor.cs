using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.UI.Extensions;
using UnityEngine;

namespace StoryEditor
{
    public class DialogEditor : Base
    {
        public Character character = null;
        public Text dialogText = null;
        public QuestionEditor questionEditor = null;
        public DialogEditor[] links = new DialogEditor[2] { null, null };
        public UILineRenderer[] lines = new UILineRenderer[2] { null, null };


        public Book.Dialog data = null;

        public DialogEditor Setup(Book.Dialog dialog)
        {
            data = dialog;

            if (dialog.character.isEmpty)
                character.Clear();
            else if (dialog.character.isPlayer)
                character.Setup(EditorPanel.current.player);
            else
                character.Setup(dialog.character);

            dialogText.SetTextAndWrap(dialog.text, true);

            questionEditor.Setup(data);

            return this;
        }

        public void UpdateLinksVisuals()
        {
            Vector3 offset = Vector3.one * 10; offset.y *= -2;

            for (int i = 0; i < lines.Length; i++)
            {
                if (links[i] != null)
                {
                    var dest = links[i].rectTransform.anchoredPosition3D + offset - (rectTransform.anchoredPosition3D + lines[i].rectTransform.anchoredPosition3D);

                    var points = lines[i].Points;
                    points[1].x = dest.x;
                    points[2].y = dest.y;
                    points[3] = dest;
                    lines[i].Points = points;

                    lines[i].enabled = true;
                }
                else lines[i].enabled = false;
            }
        }

        public void OnEditText()
        {
            gameManager.OpenPopup<Popup_InputText>().Setup(data.text, "Enter text...", str =>
            {
                if (str != null)
                {
                    data.text = str;
                    dialogText.SetTextAndWrap(data.text, true);
                }
            });
        }

        public void OnEditCharacter()
        {
            gameManager.OpenPopup<Popup_SelectCharacter>().Setup(data.character, cdata =>
            {
                if (cdata != null)
                {
                    data.character = cdata;
                    character.Clear();

                    if (cdata.isPlayer)
                        character.GetComponentInParent<Image>().sprite = gameManager.defaultPlayerSprite;
                    else if (cdata.isEmpty)
                        character.GetComponentInParent<Image>().sprite = gameManager.defaultEmptySprite;
                    else
                    {
                        character.GetComponentInParent<Image>().sprite = null;
                        character.Setup(cdata);
                    }
                }
            });
        }
    }

}
