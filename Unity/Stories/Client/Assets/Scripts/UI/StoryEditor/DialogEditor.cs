using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

namespace StoryEditor
{
    public class DialogEditor : Base
    {
        public Character character = null;
        public Text dialogText = null;
        public QuestionEditor questionEditor = null;
        public DialogEditor[] links = new DialogEditor[2] { null, null };

        public Book.Dialog data = null;

        public DialogEditor Setup(Book.Dialog dialog)
        {
            data = dialog;

            if (dialog.character.isNull)
                character.Clear();
            else if (dialog.character.isPlayer)
                character.Setup(EditorPanel.current.player);
            else
                character.Setup(dialog.character);

            dialogText.SetTextAndWrap(dialog.text, true);

            questionEditor.Setup(data);

            return this;
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
    }

}
