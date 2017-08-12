using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;

namespace SeganX.Console
{
    public class Console_Command : MonoBehaviour
    {
        public class MethodObject
        {
            public string space;
            public string name;
            public MethodInfo info;
        }

        public InputField userInput = null;

        private List<MethodObject> methods = new List<MethodObject>();

        void Awake()
        {
            var assembly = System.AppDomain.CurrentDomain.Load("Assembly-CSharp");
            var allMethods = assembly.GetTypes().SelectMany(x => x.GetMethods()).Where(y => y.GetCustomAttributes(true).OfType<ConsoleAttribute>().Any()).ToList();
            foreach (var method in allMethods)
            {
                var attribs = method.GetCustomAttributes(true);
                foreach (var attrib in attribs)
                {
                    if (attrib.IsTypeOf<ConsoleAttribute>())
                    {
                        var cattrib = attrib.As<ConsoleAttribute>();
                        if (cattrib.cmdName.IsNullOrEmpty())
                            cattrib.cmdName = method.Name.ToLower();
                        methods.Add(new MethodObject() { space = cattrib.cmdSpace, name = cattrib.cmdName, info = method });
                    }
                }
            }
        }

        public void RunCommand()
        {
            var str = userInput.text;
            Debug.Log("Execute: " + str);

            string[] cmd = str.Split(' ');
            if (cmd.Length < 2)
            {
                Debug.Log("Nothing to execute!");
                return;
            }

            var space = cmd[0].ToLower();
            var name = cmd[1].ToLower();
            var method = methods.Find(x => x.space == space && x.name == name);
            if (method == null)
            {
                Debug.Log("Command not found!");
                return;
            }
            else if (!method.info.IsStatic)
            {
                Debug.Log("Function is not static!");
                return;
            }

            var methodParams = method.info.GetParameters();
            if (methodParams.Length == 0)
            {
                method.info.Invoke(null, null);
                return;
            }
            else if (methodParams.Length != cmd.Length - 2)
            {
                var errorMsg = method.name + "(";
                for (int z = 0; z < methodParams.Length; z++)
                    errorMsg += methodParams[z].ParameterType.ToString().Replace("System.", "").Replace("Single", "float").Replace("Int32", "int") + " ";
                Debug.Log(errorMsg + ")");
                return;
            }

            var arglist = new object[methodParams.Length];
            for (int i = 0; i < arglist.Length; i++)
            {
                var methodParam = methodParams[i];

                if (methodParam.ParameterType == typeof(int))
                    arglist[i] = cmd[i + 2].ToInt();
                else if (methodParam.ParameterType == typeof(string))
                    arglist[i] = cmd[i + 2];
                else if (methodParam.ParameterType == typeof(float))
                    arglist[i] = cmd[i + 2].ToFloat();
                else
                {
                    Debug.Log("Not a type value!");
                    return;
                }
            }
            method.info.Invoke(null, arglist);
        }
    }
}