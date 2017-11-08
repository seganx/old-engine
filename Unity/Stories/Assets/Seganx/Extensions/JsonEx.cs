using UnityEngine;
using System.Collections;
using SeganX;

public static class JsonEx
{
    public static Vector4 AsVector4(this Json.Node node)
    {
        Vector4 res = Vector4.zero;
        for (int i = 0; i < node.Count && i < 4; i++)
            res[i] = node[i].AsFloat;
        return res;
    }
}
