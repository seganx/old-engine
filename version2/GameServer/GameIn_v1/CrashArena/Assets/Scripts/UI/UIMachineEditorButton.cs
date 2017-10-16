using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class UIMachineEditorButton : MonoBehaviour
{
    public Transform target = null;
    public LineRenderer line = null;

    private Vector3 offset = Vector3.zero;
    private RectTransform rectTrans = null;
    private Vector3[] positions = new Vector3[3];

    public UIMachineEditorButton Setup(Transform targetSlot, Vector3 offsetPosition)
    {
        target = targetSlot;
        offset = offsetPosition;
        return this;
    }

    // Use this for initialization
    void Start()
    {
        rectTrans = transform as RectTransform;
        line.positionCount = 3;
        positions[0] = Vector3.zero;

        if (line == null)
            line = GetComponent<LineRenderer>();
    }

    // Update is called once per frame
    void LateUpdate()
    {
        Vector2 localPoint = Vector2.zero;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(rectTrans, RectTransformUtility.WorldToScreenPoint(Camera.main, target.transform.position + offset), Camera.main, out localPoint);

        positions[2] = localPoint;
        positions[1].Set(positions[2].x, positions[0].y, 0);

        line.SetPositions(positions);
    }

    public void OnClick()
    {

    }
}
