using UnityEngine;
using System;
using System.Runtime.CompilerServices;
using System.Diagnostics;

namespace SeganX
{
    [AttributeUsage(AttributeTargets.Method)]
    public class ConsoleAttribute : Attribute
    {
        public string cmdSpace;
        public string cmdName;

        public ConsoleAttribute(string space, string altName = "")
        {
            cmdSpace = space.ToLower();
            cmdName = altName.ToLower();
        }
    }
}