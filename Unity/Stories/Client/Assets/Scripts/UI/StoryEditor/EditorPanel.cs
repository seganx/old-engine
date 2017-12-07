using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;
using SeganX;

namespace StoryEditor
{
    public class EditorPanel : Base
    {
        public Vector2 space = Vector2.one;
        public ScrollRect scroller = null;
        public DialogEditor prefab = null;

        private Vector2Int level = Vector2Int.zero;

        public EditorPanel Setup(Book book)
        {
            current = book;

            //  remove current dialogs
            foreach (var item in dialogs)
                Destroy(item.gameObject);
            dialogs.Clear();

            //  create dialog boxes
            foreach (var item in book.dialogs)
                dialogs.Add(prefab.Clone<DialogEditor>(scroller.content).Setup(item));

            //  link dialog boxes
            foreach (var item in dialogs)
                if (item.data.next.HasContent())
                    item.links[0] = dialogs.Find(x => x.data.name == item.data.next);
                else for (int i = 0; i < item.data.questions.Count && i < 2; i++)
                        item.links[i] = dialogs.Find(x => x.data.name == item.data.questions[i].onclick_dialog);


            level = Vector2Int.zero;
            UpdatePosition(dialogs[0], 0);
            return this;
        }

        private void UpdatePosition(DialogEditor dialog, int index)
        {
            if (dialog == null) return;
            dialog.rectTransform.anchoredPosition = new Vector2(index * space.x, level.y * space.y);
            foreach (var item in dialog.links)
            {
                if (item != null)
                {
                    UpdatePosition(item, index + 1);
                    level.y--;
                }
            }
        }

        // Use this for initialization
        void Start()
        {
            var mybook = new Book();

            var d = new Book.Dialog();
            d.name = "d0";
            d.character = new CharacterData() { family = "sajad", body = "body_2", face = "face_1", hair = "hair_3", name = "salman" };
            d.text = "Hello baby!!!";
            d.next = "d1";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "d1";
            d.character = new CharacterData() { family = "sajad", body = "body_1", face = "face_1", hair = "hair_1", name = "saeed" };
            d.text = "Hello dear!!!";

            var q = new Book.Question();
            q.text = "question 1";
            q.onclick_dialog = "path1";
            d.questions.Add(q);

            q = new Book.Question();
            q.text = "question 2";
            q.onclick_dialog = "path2";
            d.questions.Add(q);

            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "path1";
            d.character = new CharacterData() { family = "sajad", body = "body_0", face = "face_2", hair = "hair_3", name = "salman" };
            d.text = "path 1!!!";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "path2";
            d.character = new CharacterData() { family = "sajad", body = "body_2", face = "face_1", hair = "hair_2", name = "salman" };
            d.text = "path 2!!!";
            mybook.dialogs.Add(d);

            Setup(mybook);
        }

        // Update is called once per frame
        void Update()
        {

        }


        //////////////////////////////////////////////////////
        // static memebers
        //////////////////////////////////////////////////////
        public static Book current = null;
        public static List<DialogEditor> dialogs = new List<DialogEditor>();
    }
}

