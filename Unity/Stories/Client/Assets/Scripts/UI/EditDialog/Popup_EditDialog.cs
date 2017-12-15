using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

namespace StoryEditor
{
    public class Popup_EditDialog : GameState
    {
        public Image background = null;
        public InputField dialogName = null;
        public Character character = null;
        public InputField dialogText = null;
        public GameObject questionNonePanel = null;
        public GameObject questionPanel = null;
        public InputField[] questionText = null;
        public Text[] questionLink = null;
        public Text defaultLink = null;

        private System.Action<bool> callbackFunc = null;
        private Book.Dialog data = null;
        private string nextPage = "";
        private CharacterData charData = null;


        public Popup_EditDialog Setup(Book.Dialog dialog, System.Action<bool> callback)
        {
            data = dialog;
            callbackFunc = callback;
            charData = dialog.character;

            //background.sprite = data.background;
            dialogName.text = dialog.name;
            UpdateFace();
            dialogText.text = dialog.text;
            defaultLink.text = "Next Page: " + (nextPage = dialog.next);
            if (data.questions.Count > 1)
            {
                questionNonePanel.SetActive(false);
                questionPanel.SetActive(true);
                questionText[0].text = data.questions[0].text;
                questionText[1].text = data.questions[1].text;
                questionLink[0].text = data.questions[0].onclick_dialog;
                questionLink[1].text = data.questions[1].onclick_dialog;
            }
            else
            {
                questionNonePanel.SetActive(true);
                questionPanel.SetActive(false);
            }
            return this;
        }

        public void UpdateFace()
        {
            var image = character.GetComponentInChildren<Image>(true);

            if (charData.isEmpty)
            {
                character.Clear();
                image.gameObject.SetActive(true);
                image.sprite = gameManager.characterDefaults.Empty;
            }
            else if (charData.isPlayer)
            {
                character.Clear();
                image.gameObject.SetActive(true);
                image.sprite = gameManager.characterDefaults.Player;
            }
            else
            {
                image.gameObject.SetActive(false);
                character.Setup(charData);
            }
        }

        public void OnEditBackground()
        {

        }

        public void OnEditCharacter()
        {
            gameManager.OpenPopup<Popup_SelectCharacter>().Setup(data.character, cdata =>
            {
                if (cdata != null)
                {
                    charData = cdata;
                    UpdateFace();
                }
            });
        }

        public void OnEditLink(int index)
        {
            if (index == -1)
            {
                gameManager.OpenPopup<Popup_SelectDialog>().Setup(EditorPanel.current, data, nextPage, bdata =>
                {
                    defaultLink.text = "Next Page: " + (nextPage = bdata.name == "none" ? "" : bdata.name);
                });
            }
            else
            {
                gameManager.OpenPopup<Popup_SelectDialog>().Setup(EditorPanel.current, data, questionLink[index].text, bdata =>
                {
                    questionLink[index].text = (nextPage = bdata.name == "none" ? "" : bdata.name);
                });
            }
        }

        public void OnConfirm(bool isOk)
        {
            if (isOk)
            {
                var newname = dialogName.text.Trim() != "none" ? dialogName.text.Trim().CleanForPersian() : "";
                if (newname.Length > 1 && EditorPanel.current.dialogs.Find(x => x.name == newname) == null)
                    data.name = newname;

                data.character = charData;
                data.text = dialogText.text.CleanForPersian();
                data.questions.Clear();

                if (questionPanel.activeSelf && questionLink[0].text.Length > 0 && questionLink[1].text.Length > 0)
                {
                    data.next = "";
                    data.questions.Add(new Book.Question() { text = questionText[0].text.CleanForPersian(), onclick_dialog = questionLink[0].text });
                    data.questions.Add(new Book.Question() { text = questionText[1].text.CleanForPersian(), onclick_dialog = questionLink[1].text });
                }
                else
                {
                    data.next = nextPage;
                    data.questions.Clear();
                }
            }

            if (callbackFunc != null)
                callbackFunc(isOk);

            base.Back();
        }

        public override void Back()
        {
            OnConfirm(false);
        }
    }
}