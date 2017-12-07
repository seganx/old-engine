using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

namespace StoryEditor
{
    public class QuestionEditor : Base
    {
        public Button addButton = null;
        public Button removeButton = null;
        public GameObject[] labels = null;

        private Book.Dialog data = null;
        public QuestionEditor Setup(Book.Dialog dialogData)
        {
            data = dialogData;
            if (data.questions.Count > 1)
                AddQuestions();
            else
                RemoveQuestions();
            return this;
        }

        public void AddQuestions()
        {
            addButton.gameObject.SetActive(false);
            removeButton.gameObject.SetActive(true);

            if (data.questions.Count < 2)
            {
                data.questions.Add(new Book.Question());
                data.questions.Add(new Book.Question());
            }

            for (int i = 0; i < labels.Length; i++)
            {
                labels[i].SetActive(true);
                labels[i].GetComponentInChildren<Text>().SetTextAndWrap(data.questions[i].text);
            }
        }

        public void RemoveQuestions()
        {
            addButton.gameObject.SetActive(true);
            removeButton.gameObject.SetActive(false);
            data.questions.Clear();

            foreach (var item in labels)
                item.SetActive(false);
        }

        public void OnEditLabel(int index)
        {
            gameManager.OpenPopup<Popup_InputText>().Setup(data.questions[index].text, "Enter text...", str =>
            {
                if (str != null)
                {
                    data.questions[index].text = str;
                    labels[index].GetComponentInChildren<Text>().SetTextAndWrap(data.questions[index].text, true);
                }
            });
        }
    }
}

