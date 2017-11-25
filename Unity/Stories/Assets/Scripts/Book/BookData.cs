using System.Collections.Generic;

public class BookData
{
    public class Question
    {
        public string text = "";
        public string onclick_page = "";
        public string onclick_dialog = "";
        public string onclick_param = "";
    }

    public class Dialog
    {
        public CharacterData character = new CharacterData();
        public string text = "";
        public List<Question> questions = new List<Question>();
    }

    public class Page
    {
        public string name = "";
        public string background = "";
        public List<Dialog> dialogs = new List<Dialog>();
    }

    public int Id = 0;
    public int chapter = 0;
    public string border = "";
    public string dialog = "";
    public CharacterData player = new CharacterData();
    public List<Page> pages = new List<Page>();
}
