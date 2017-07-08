using UnityEngine;
using System.Collections;

public static class GameObjectEx
{
    public static RectTransform GetRectTransform(this GameObject self)
    {
        return self.transform as RectTransform;
    }

    public static GameObject Clone(this GameObject self)
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.transform.SetParent(self.transform.parent);
        res.transform.CopyValuesFrom(self.transform);
        return res;
    }

    public static T Clone<T>(this GameObject self) where T : Component
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        if (self.transform.parent != null)
            res.transform.SetParent(self.transform.parent);
        res.transform.CopyValuesFrom(self.transform);
        return res.GetComponent<T>();
    }

    public static GameObject Clone(this GameObject self, Transform similarTo)
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.transform.SetParent(similarTo.parent);
        res.transform.CopyValuesFrom(similarTo);
        return res;
    }

    public static GameObject Clone(this GameObject self, Transform similarTo, Transform parent)
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.transform.SetParent(parent);
        res.transform.CopyValuesFrom(similarTo);
        return res;
    }
}
