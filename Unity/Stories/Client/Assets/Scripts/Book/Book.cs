using System.Collections.Generic;

public class Book
{
    public class Question
    {
        public string text = "";
        public string onclick_dialog = "";
        public string onclick_param = "";
    }

    public class Dialog
    {
        public string name = "";
        public string background = "";
        public CharacterData character = new CharacterData();
        public string text = "";
        public string next = "";
        public List<Question> questions = new List<Question>();
    }

    public int Id = 0;
    public int chapter = 0;
    public string border = "";
    public string dialog = "";
    public CharacterData player = new CharacterData();
    public List<Dialog> dialogs = new List<Dialog>();
}
