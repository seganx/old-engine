using UnityEngine;
using System.Collections;

public static class JsonEx
{
    public static Vector4 AsVector4(this JSONNode node)
    {
        Vector4 res = Vector4.zero;
        for (int i = 0; i < node.Count && i < 4; i++)
            res[i] = node[i].AsFloat;
        return res;
    }
}
