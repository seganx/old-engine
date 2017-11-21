using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIMover : Base
{
    public float speed = 20;
    public float freezeThreshold = 4;
    public Vector3 destPosition;
    public Vector3 currPosition;

    public UIMover Setup(Vector3 position)
    {
        rectTransform.anchoredPosition3D = destPosition = currPosition = position;
        return this;
    }

    // Update is called once per frame
    void Update()
    {
        if (currPosition == destPosition) return;

        var diff = destPosition - currPosition;
        currPosition += diff * speed * Time.deltaTime;
        if (diff.sqrMagnitude < freezeThreshold)
            currPosition = destPosition;

        rectTransform.anchoredPosition3D = currPosition;
    }
}
