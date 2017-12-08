public class CharacterData
{
    public string family = "";
    public string name = "";
    public string body = "";
    public string face = "";
    public string hair = "";

    public bool isPlayer { get { return family == "player" || name == "player"; } }
    public bool isEmpty { get { return family.IsNullOrEmpty() || name.IsNullOrEmpty(); } }

    public CharacterData() { }

    public CharacterData(string _family, string _name, string _body, string _face, string _hair)
    {
        family = _family; name = _name; body = _body; face = _face; hair = _hair;
    }

    public CharacterData CopyFrom(CharacterData src)
    {
        family = src.family;
        name = src.name;
        body = src.body;
        face = src.face;
        hair = src.hair;
        return this;
    }

    public bool IsSameAs(CharacterData other)
    {
        return family == other.family && name == other.name && body == other.body && face == other.face && hair == other.hair;
    }

    public static CharacterData Empty { get { return new CharacterData(); } }
    public static CharacterData Player { get { return new CharacterData() { family = "player", name = "player" }; } }
}
