using System.Collections.Generic;

[System.Serializable]
public class Book
{
    [System.Serializable]
    public class SaveData
    {
        public CharacterData character = new CharacterData();
        public string section = "";
        public string page = "";
    }

    [System.Serializable]
    public class Question
    {
        public string text = "";
        public string onclick_section = "";
        public string onclick_dialog = "";
    }

    [System.Serializable]
    public class Page
    {
        public string name = "";
        public bool usePlayerCharacter = false;
        public CharacterData character = new CharacterData();
        public string text = "";
        public List<Question> questions = new List<Question>();
    }

    [System.Serializable]
    public class Section
    {
        public string name = "";
        public string background = "";
        public List<Page> pages = new List<Page>();
    }

    public string name = "";
    public string author = "";
    public string cover = "";
    public string border = "";
    public string dialog = "";
    public CharacterData character = new CharacterData();
    public List<Section> sections = new List<Section>();
}
