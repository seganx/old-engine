using UnityEngine;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;

public static class DebugEx
{
    public static string GetStringDebug(this object self)
    {
        if (self == null) return "[null]";
        System.Type type = self.GetType();

        if (type.IsValueType || type.IsEnum || type.IsPrimitive || type == typeof(string))
            return "[" + self.ToString() + "]";

        if (type.IsArray)
        {
            string res = "{";
            var arr = self as System.Array;
            foreach (var a in arr)
                res += GetStringDebug(a) + " ";
            return (res.Length > 3 ? res.Remove(res.Length - 1) : res) + "}";
        }

        if (type.IsGenericType)
        {
            string res = "{";
            var arr = self as ICollection;
            foreach (var a in arr)
                res += GetStringDebug(a) + " ";
            return (res.Length > 3 ? res.Remove(res.Length - 1) : res) + "}";
        }

        if (type.IsClass)
        {
            string res = "{";
            var members = type.GetMembers();
            foreach (var member in members)
            {
                if (member.MemberType == MemberTypes.Field)
                {
                    var field = member as FieldInfo;
                    res += field.Name + GetStringDebug(field.GetValue(self)) + " ";
                }
                if (member.MemberType == MemberTypes.Property)
                {
                    var prop = member as PropertyInfo;
                    res += prop.Name + GetStringDebug(prop.GetValue(self, null)) + " ";
                }
            }
            res = res.Remove(res.Length - 1) + "}";

            if (res.Length > 100)
                return "\n" + res;
            else
                return res;
        }

        return "";
    }
}
