public class CharacterData
{
    public string family = "";
    public string name = "";
    public string body = "";
    public string face = "";
    public string hair = "";

    public bool isPlayer { get { return family.IsNullOrEmpty() || name.IsNullOrEmpty(); } }

    public CharacterData CopyFrom(CharacterData src)
    {
        family = src.family;
        name = src.name;
        body = src.body;
        face = src.face;
        hair = src.hair;
        return this;
    }
}
