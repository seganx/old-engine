using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SeganX
{
    public class InputManager : MonoBehaviour
    {
        public class Button
        {
            public float pressTime = 0;
            public bool isPointerDown = false;
            public bool isPointerHold = false;
            public bool isPointerUp = true;
            public bool isPointerClick { get { return isPointerDown && !isPointerHold; } }

            public void OnPointerDown()
            {
                isPointerUp = false;
                isPointerDown = true;
            }

            public void OnPointerUp()
            {
                pressTime = 0;
                isPointerUp = true;
                isPointerDown = isPointerHold = false;
            }

            public void OnLateUpdate()
            {
                if (isPointerDown)
                {
                    isPointerHold = true;
                    pressTime += Time.deltaTime;
                }
            }
        }

        public class Joystick : Button
        {
            public float verticalValue = 0;
            public float horizontalValue = 0;
        }

        public class SteeringWheel : Button
        {
            public float relativeAngle = 0;
            public float totalAngle = 0;
        }

        public static SteeringWheel Steering = new SteeringWheel();
        public static Joystick JoystickLeft = new Joystick();
        public static Joystick JoystickRight = new Joystick();
        public static Button Jump = new Button();
        public static Button Fire = new Button();
        public static Button Accelerate = new Button();
        public static Button Boost = new Button();
        public static Button Break = new Button();
    }
}