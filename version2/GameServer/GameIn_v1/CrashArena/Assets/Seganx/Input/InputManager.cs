using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class InputManager : MonoBehaviour
    {
        public class Button
        {
            public bool isPointerDown = false;
            public bool isPointerHold = false;
            public bool isPointerUp = true;
            public bool isPointerClick { get { return isPointerDown && !isPointerHold; } }
        }

        public class Joystick : Button
        {
            public float verticalValue = 0;
            public float horizontalValue = 0;
        }

        public static Button Jump = new Button();
        public static Button Fire = new Button();
        public static Joystick JoystickLeft = new Joystick();
        public static Joystick JoystickRight = new Joystick();

        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }
    }
}