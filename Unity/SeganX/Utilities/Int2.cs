namespace SeganX
{
    public class Int2
    {
        public int x;
        public int y;

        public Int2(int _x, int _y)
        {
            x = _x;
            y = _y;
        }

        public Int2(Int2 int2)
        {
            x = int2.x;
            y = int2.y;
        }
    }
}
