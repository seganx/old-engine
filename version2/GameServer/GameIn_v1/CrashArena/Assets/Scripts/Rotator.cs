﻿using UnityEngine;
using System.Collections;

public class Rotator : MonoBehaviour
{
    public float speed = 0;

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        transform.Rotate(0, 0, speed);
    }
}
