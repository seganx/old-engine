using System.Collections.Generic;

public class ProfileData
{
    public class ReadingBook
    {
        public int id = 0;
        public int chapter = 0;
        public string page = "";
        public string dialog = "";
        public CharacterData character = new CharacterData();
        public Dictionary<string, int> param = new Dictionary<string, int>();
    }

    public string openId = "";
    public string nickname = "";
    public int score = 0;
    public int keys = 0;
    public int gems = 0;
    public List<ReadingBook> readingBooks = new List<ReadingBook>();
}
