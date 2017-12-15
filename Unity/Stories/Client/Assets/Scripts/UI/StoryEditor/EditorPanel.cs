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
        public DialogItem prefab = null;

        private Vector2 contentSize = Vector2.zero;
        private Vector2Int positionLevel = Vector2Int.zero;
        private List<DialogItem> tempList = new List<DialogItem>();

        public EditorPanel Setup(Book book)
        {
            current = book;

            //  remove current dialogs
            foreach (var item in dialogs)
                Destroy(item.gameObject);
            dialogs.Clear();

            //  create dialog boxes
            foreach (var item in book.dialogs)
                dialogs.Add(prefab.Clone<DialogItem>(scroller.content).Setup(item));

            //  link dialog boxes
            foreach (var item in dialogs)
                UpdateLink(item);

            return Referesh();
        }

        public EditorPanel Referesh()
        {
            if (dialogs.Count < 1) return this;

            tempList.Clear();
            tempList.AddRange(dialogs);

            contentSize = Vector2.zero;
            positionLevel = Vector2Int.zero;
            UpdateLinkedPosition(dialogs[0], 0);

            while (tempList.Count > 0)
            {
                positionLevel.x++;
                UpdateLinkedPosition(tempList[0], 0);
            }
            scroller.content.sizeDelta = contentSize;

            foreach (var item in dialogs)
                item.UpdateLinksVisuals();

            return this;
        }

        private void UpdateLinkedPosition(DialogItem dialog, int index)
        {
            if (dialog == null) return;
            tempList.Remove(dialog);
            dialog.rectTransform.anchoredPosition = new Vector2(positionLevel.x * space.x, index * space.y);

            UpdateLinkedPosition(dialog.links[0], index - 1);
            if (dialog.links[1] != null) positionLevel.x++;
            UpdateLinkedPosition(dialog.links[1], index - 1);

            if (contentSize.x <= positionLevel.x * space.x + space.x * 2)
                contentSize.x = positionLevel.x * space.x + space.x * 2;
            if (contentSize.y <= -index * space.y + space.y * 2)
                contentSize.y = -index * space.y + space.y * 2;
        }

        // Use this for initialization
        void Start()
        {
            var mybook = new Book();

            var d = new Book.Dialog();
            d.name = "z0";
            d.character = new CharacterData() { family = "sajad", body = "body_2", face = "face_1", hair = "hair_3", name = "salman" };
            d.text = "Hello baby!!!";
            d.next = "z1";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "z1";
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
            d.next = "z02";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "z01";
            d.character = new CharacterData() { family = "sajad", body = "body_2", face = "face_1", hair = "hair_2", name = "salman" };
            d.text = "unlinked!!";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "z02";
            d.character = new CharacterData() { family = "sajad", body = "body_1", face = "face_1", hair = "hair_1", name = "saeed" };
            d.text = "unlinked with question";

            q = new Book.Question();
            q.text = "question 1";
            q.onclick_dialog = "sajad";
            d.questions.Add(q);

            q = new Book.Question();
            q.text = "question 2";
            q.onclick_dialog = "_path2";
            d.questions.Add(q);

            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "sajad";
            d.character = new CharacterData() { family = "sajad", body = "body_0", face = "face_2", hair = "hair_3", name = "salman" };
            d.text = "path 1!!!";
            mybook.dialogs.Add(d);

            d = new Book.Dialog();
            d.name = "_path2";
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
        public static List<DialogItem> dialogs = new List<DialogItem>();

        public static void UpdateLink(DialogItem item)
        {
            item.links[0] = item.links[1] = null;
            if (item.data.next.HasContent())
                item.links[0] = dialogs.Find(x => x.data.name == item.data.next);
            else for (int i = 0; i < item.data.questions.Count && i < 2; i++)
                    item.links[i] = dialogs.Find(x => x.data.name == item.data.questions[i].onclick_dialog);
        }
    }
}

