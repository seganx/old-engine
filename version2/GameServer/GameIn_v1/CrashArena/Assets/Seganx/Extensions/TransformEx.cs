using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

public static class TransformEx
{
    public static Transform CopyValuesFrom(this Transform self, Transform source)
    {
        self.localScale = source.localScale;
        self.localRotation = source.localRotation;
        self.localPosition = source.localPosition;

        if (self is RectTransform && source is RectTransform)
        {
            RectTransform rself = self as RectTransform;
            RectTransform rsrce = source as RectTransform;

            rself.anchoredPosition3D = rsrce.anchoredPosition3D;

            rself.anchorMax = rsrce.anchorMax;
            rself.anchorMin = rsrce.anchorMin;
            rself.sizeDelta = rsrce.sizeDelta;
        }

        return self;
    }

    public static int GetActiveChildCount(this Transform self)
    {
        int res = 0;
        for (int i = 0; i < self.childCount; i++)
        {
            if (self.GetChild(i).gameObject.activeSelf)
                res++;
        }
        return res;
    }

    public static Transform RemoveChildren(this Transform self, int startIndex = 0, int count = -1)
    {
        if (count == 0) return self;

        if (count > 0)
        {
            int endIndex = Mathf.Min(count + startIndex, self.childCount);
            for (int i = startIndex; i < endIndex; i++)
                GameObject.Destroy(self.GetChild(i).gameObject);
        }
        else
        {
            for (int i = startIndex; i < self.childCount; i++)
                GameObject.Destroy(self.GetChild(i).gameObject);
        }
        return self;
    }

    public static Transform RemoveChildrenBut(this Transform self, int except)
    {
        for (int i = 0; i < self.childCount; i++)
            if (except != i)
                GameObject.Destroy(self.GetChild(i).gameObject);
        return self;
    }

    //! this will disable all children but activate a child specified by index
    public static Transform SetActiveChild(this Transform self, int index)
    {
        for (int i = 0; i < self.childCount; i++)
            self.GetChild(i).gameObject.SetActive(i == index);
        return self;
    }

    //! return index of first activated child. return -1 if no active child found
    public static int GetActiveChild(this Transform self)
    {
        for (int i = 0; i < self.childCount; i++)
            if (self.GetChild(i).gameObject.activeSelf)
                return i;
        return -1;
    }

    public static RectTransform GetRectTransform(this Transform self)
    {
        return self as RectTransform;
    }

    public static Vector3 GetAnchordPosition(this Transform self)
    {
        return (self as RectTransform).anchoredPosition3D;
    }

    public static RectTransform SetAnchordPosition(this Transform self, Vector3 pos)
    {
        (self as RectTransform).anchoredPosition3D = pos;
        return self as RectTransform;
    }

    public static RectTransform SetAnchordPosition(this Transform self, float x, float y, float z)
    {
        (self as RectTransform).anchoredPosition3D = new Vector3(x, y, z);
        return self as RectTransform;
    }

    public static RectTransform SetAnchordPositionX(this Transform self, float x)
    {
        Vector3 pos = self.GetAnchordPosition();
        pos.x = x;
        self.SetAnchordPosition(pos);
        return self as RectTransform;
    }

    public static RectTransform SetAnchordPositionY(this Transform self, float y)
    {
        Vector3 pos = self.GetAnchordPosition();
        pos.y = y;
        self.SetAnchordPosition(pos);
        return self as RectTransform;
    }

    public static RectTransform SetAnchordHeight(this Transform self, float height)
    {
        var size = (self as RectTransform).sizeDelta;
        size.y = height;
        (self as RectTransform).sizeDelta = size;
        return self as RectTransform;
    }

    public static RectTransform SetAnchordWidth(this Transform self, float width)
    {
        var size = (self as RectTransform).sizeDelta;
        size.x = width;
        (self as RectTransform).sizeDelta = size;
        return self as RectTransform;
    }

    public static Transform SetParent(this Transform self, Transform parent, bool holdPosition, bool holdScale, bool holdRotation)
    {
        Vector3 lastPos = self.position;
        Quaternion lastRotation = self.rotation;
        Vector3 lastScale = self.localScale;

        self.SetParent(parent, false);

        if (holdScale) self.localScale = lastScale;
        if (holdRotation) self.rotation = lastRotation;
        if (holdPosition) self.position = lastPos;

        return self;
    }

    public static Transform SetParent(this Transform self, Transform parent, Vector3 localPosition, Vector3 localScale)
    {
        self.SetParent(parent, false);
        self.localScale = localScale;
        self.localPosition = localPosition;
        return self;
    }

    public static RectTransform AttachToCanvas(this Transform self, string canvasName)
    {
        RectTransform rectTransform = self.GetRectTransform();
        rectTransform.SetParent(GameObject.Find(canvasName).transform, false);
        return rectTransform;
    }

    public static T GetComponentInParent<T>(this Transform self, bool inactives) where T : Component
    {
        if (inactives == false)
            return self.GetComponentInParent<T>();

        while (self != null)
        {
            T res = self.GetComponent<T>();
            if (res != null)
                return res;
            self = self.parent;
        }

        return null;
    }

    public static T GetChild<T>(this Transform self, int index) where T : Component
    {
        var res = self.GetChild(index).GetComponent<T>();
        return res == null ? self.GetChild(index).GetComponentInChildren<T>() : res;
    }

    public static Transform FindChildRecursive(this Transform self, string childName, bool justActivates = false)
    {
        Transform child = self.FindChild(childName);
        if (child != null)
        {
            if (justActivates)
            {
                if (child.gameObject.activeInHierarchy)
                    return child;
            }
            else return child;
        }

        for (int idx = 0; idx < self.childCount; ++idx)
            if ((child = FindChildRecursive(self.GetChild(idx), childName, justActivates)) != null)
                return child;

        return null;
    }

    //
    // Summary:
    //     Rotate transform to look at the target. NOTE that transform and target should have same anchors!
    //
    // Parameters:
    //   target:
    //     transform will look at the target in 2D space.
    //
    // Returns:
    //     return self transform
    public static RectTransform LookAt2D(this RectTransform self, Vector2 target)
    {
        Vector2 dis = target - self.anchoredPosition;
        self.localEulerAngles = new Vector3(0, 0, Mathf.Atan2(dis.y, dis.x) * Mathf.Rad2Deg);
        return self;
    }

#if OFF
    public static bool IsInViewport2D(this RectTransform self, ScrollListRectInfo viewport)
    {
        Vector2 pos = new Vector2(self.position.x, self.position.y);
        Vector2 max = pos + (Vector2)(self.parent.localToWorldMatrix * self.rect.max);
        Vector2 min = pos + (Vector2)(self.parent.localToWorldMatrix * self.rect.min);
        return min.y <= viewport.TopEdge && max.y >= viewport.BottomEdge && min.x <= viewport.RightEdge && max.x >= viewport.LeftEdge;
    }
#endif
}
