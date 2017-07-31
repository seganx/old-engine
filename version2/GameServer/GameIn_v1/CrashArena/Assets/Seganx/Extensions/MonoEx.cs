using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

public static class MonoEx
{
    #region basic types
    public static bool IsTypeOf<T>(this object self)
    {
        return self != null && self is T;
    }

    public static T As<T>(this object self) where T : class
    {
        return self as T;
    }

    public static bool Between(this float x, float x1, float x2)
    {
        return x >= x1 && x <= x2;
    }

    public static bool Between(this int x, int x1, int x2)
    {
        return x >= x1 && x <= x2;
    }
    #endregion

    #region string
    public static bool IsNullOrEmpty(this string self)
    {
        return string.IsNullOrEmpty(self);
    }

    public static bool HasContent(this string self)
    {
        return !string.IsNullOrEmpty(self);
    }

    public static bool HasContent(this string self, int minLength)
    {
        return !string.IsNullOrEmpty(self) && self.Length >= minLength;
    }

    public static byte[] GetBytes(this string self)
    {
        return System.Text.Encoding.UTF8.GetBytes(self);
    }

    public static string EscapeURL(this string self)
    {
        return WWW.EscapeURL(self).Replace("+", "%20");
    }

    public static string BuildPath(this string self)
    {
        if (Application.platform == RuntimePlatform.WindowsEditor)
            return self.Replace("/", "\\");
        else
            return self.Replace("\\", "/");
    }

    public static int ToInt(this string self, int defaultValue = 0)
    {
        if (string.IsNullOrEmpty(self))
            return defaultValue;
        int res = 0;
        if (int.TryParse(self, out res))
            return res;
        else return defaultValue;
    }

    public static float ToFloat(this string self, float defaultValue = 0)
    {
        if (string.IsNullOrEmpty(self))
            return defaultValue;
        float res = 0;
        if (float.TryParse(self, out res))
            return res;
        else return defaultValue;
    }

    public static T ToEnum<T>(this string self, T defaultValue)
    {
        if (string.IsNullOrEmpty(self))
            return defaultValue;
        try
        {
            T res = (T)System.Enum.Parse(typeof(T), self, true);
            return System.Enum.IsDefined(typeof(T), res) ? res : defaultValue;
        }
        catch { }
        return defaultValue;
    }

    public static int FindDigit(this string self, int startIndex = 0)
    {
        if (self.HasContent())
            for (int i = 0; i < self.Length; i++)
                if (char.IsDigit(self[i]))
                    return i;
        return -1;
    }

    public static bool ContainsAny(this string self, string[] items)
    {
        if (self.IsNullOrEmpty() || items.IsNullOrEmpty()) return false;
        foreach (var item in items)
            if (self.Contains(item))
                return true;
        return false;
    }

    public static bool ContainsAll(this string self, string[] items)
    {
        if (self.IsNullOrEmpty() || items.IsNullOrEmpty()) return false;
        foreach (var item in items)
            if (self.Contains(item) == false)
                return false;
        return true;
    }

    public static string GetWithoutBOM(this string self)
    {
        MemoryStream memoryStream = new MemoryStream(self.GetBytes());
        StreamReader streamReader = new StreamReader(memoryStream, true);
        string result = streamReader.ReadToEnd();
        streamReader.Close();
        memoryStream.Close();
        return result;
    }

    public static string ComputeMD5(this string self, string salt)
    {
        var md5 = System.Security.Cryptography.MD5.Create();
        byte[] inputBytes = System.Text.Encoding.ASCII.GetBytes(self + salt);
        byte[] hashBytes = md5.ComputeHash(inputBytes);

        var res = new System.Text.StringBuilder();
        for (int i = 0; i < hashBytes.Length; i++)
            res.Append(hashBytes[i].ToString("X2"));

        return res.ToString();
    }

    public static int GetBreakCount(this string self)
    {
        var lines = self.Split('\n');
        return lines.Length;
    }
    #endregion

    #region array
    public static bool HasOneItem(this System.Array self)
    {
        return self != null && self.Length == 1;
    }

    public static bool HasItem(this System.Array self)
    {
        return self != null && self.Length > 0;
    }

    public static bool IsNullOrEmpty(this System.Array self)
    {
        return self == null || self.Length < 1;
    }

    public static bool Contains(this System.Array self, object item)
    {
        if (self.IsNullOrEmpty() || item == null) return false;
        foreach (var i in self)
            if (item.Equals(i))
                return true;
        return false;
    }
    #endregion

    #region system
    public static string GetPersianDateTime(this System.DateTime dateTime)
    {
        PersianCalendar pc = new PersianCalendar();
        try { return string.Format("{0}/{1}/{2} {3}:{4}", pc.GetYear(dateTime), pc.GetMonth(dateTime), pc.GetDayOfMonth(dateTime), pc.GetHour(dateTime), pc.GetMinute(dateTime)); }
        catch { }
        return "";
    }
    #endregion

}
