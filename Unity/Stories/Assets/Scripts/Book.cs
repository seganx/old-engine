using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Book
{
    public class Meta
    {
        public string name;
        public string author;
        public string coverUri;
    }

    public class Page
    {
        public class Item
        {
            public Vector2 position;
            public string imageUri;
        }



        public string name;
        public string background;
    }
}
