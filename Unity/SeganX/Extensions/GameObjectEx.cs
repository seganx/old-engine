﻿using UnityEngine;
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
        res.RefreshMaterials();
        res.transform.SetParent(self.transform.parent);
        res.transform.CopyValuesFrom(self.transform);
        return res;
    }

    public static T Clone<T>(this GameObject self) where T : Component
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.RefreshMaterials();
        if (self.transform.parent != null)
            res.transform.SetParent(self.transform.parent);
        res.transform.CopyValuesFrom(self.transform);
        return res.GetComponent<T>();
    }

    public static T Clone<T>(this Component self) where T : Component
    {
        return self.gameObject.Clone<T>();
    }

    public static T Clone<T>(this Object self) where T : Component
    {
        return (self as GameObject).Clone<T>();
    }

    public static T Clone<T>(this Component self, Transform parent, bool worldPositionStays = false) where T : Component
    {
        T res = self.gameObject.Clone<T>();
        res.transform.SetParent(parent, worldPositionStays);
        return res;
    }

    public static T Clone<T>(this Object self, Transform parent, bool worldPositionStays = false) where T : Component
    {
        T res = (self as GameObject).Clone<T>();
        res.transform.SetParent(parent, worldPositionStays);
        return res;
    }

    public static GameObject Clone(this GameObject self, Transform similarTo)
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.RefreshMaterials();
        res.transform.SetParent(similarTo.parent);
        res.transform.CopyValuesFrom(similarTo);
        return res;
    }

    public static GameObject Clone(this GameObject self, Transform similarTo, Transform parent)
    {
        GameObject res = GameObject.Instantiate(self);
        res.name = self.name;
        res.RefreshMaterials();
        res.transform.SetParent(parent);
        res.transform.CopyValuesFrom(similarTo);
        return res;
    }

    public static void DestroyNow(this GameObject self)
    {
        self.transform.SetParent(null);
        self.SetActive(false);
        GameObject.Destroy(self);
    }

    public static GameObject RefreshMaterials(this GameObject self)
    {
        var renderers = self.GetComponentsInChildren<Renderer>(true);
        foreach (var render in renderers)
            foreach (var mat in render.materials)
                mat.RefreshMaterial();

        var graphics = self.GetComponentsInChildren<UnityEngine.UI.Graphic>(true);
        foreach (var graphic in graphics)
        {
            graphic.material.RefreshMaterial();
            graphic.materialForRendering.RefreshMaterial();
        }
        return self;
    }

    public static Material RefreshMaterial(this Material self)
    {
        if (self.shader == null || self.hideFlags != HideFlags.None) return self;
        var shader = Shader.Find(self.shader.name);
        if (shader == null) return self;
        self.shader = shader;
        return self;
    }
}
