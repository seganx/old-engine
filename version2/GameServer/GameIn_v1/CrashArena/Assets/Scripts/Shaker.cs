using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shaker : MonoBehaviour
{
    public Vector3 range = Vector3.zero;
    public float speed = 0;

    private Vector3 basePosition = Vector3.zero;

    // Use this for initialization
    void Start()
    {
        basePosition = transform.localPosition;
    }

    // Update is called once per frame
    void Update()
    {
        transform.localPosition = basePosition + range * Mathf.Sin(Time.time * speed);
    }
}
