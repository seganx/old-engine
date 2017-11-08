﻿using UnityEngine;
using UnityEngine.UI;
using System.Collections;

namespace SeganX.Console
{
    public class Console_FPS : MonoBehaviour
    {
        public Text fpsLabel = null;
        public bool updateColor = true; // Do you want the color to change if the FPS gets low
        public float frequency = 0.5F;  // The update frequency of the fps
        public int nbDecimal = 1;       // How many decimal do you want to display

        private float accum = 0f;           // FPS accumulated over the interval
        private int frames = 0;             // Frames drawn over the interval
        private float deltaTime = 0;

        void OnEnable() { StartCoroutine(FPS()); }
        void OnDisable() { StopCoroutine(FPS()); }

        // Use this for initialization
        void Start()
        {
            if (fpsLabel == null)
                fpsLabel = GetComponent<Text>();
        }

        // Update is called once per frame
        void Update()
        {
            accum += Time.timeScale / Time.deltaTime;
            ++frames;
            deltaTime = Time.deltaTime;
        }

        IEnumerator FPS()
        {
            // Infinite loop executed every "frequency" seconds.
            while (true)
            {
                // Update the FPS
                float fps = accum / frames;

                string str = "FPS: " + fps.ToString("f" + Mathf.Clamp(nbDecimal, 0, 10)) + "\n";
                str += "DT: " + deltaTime.ToString("f6");

                fpsLabel.text = str;

                //Update the color
                fpsLabel.color = (fps >= 30) ? Color.green : ((fps < 10) ? Color.red : Color.yellow);

                accum = 0.0F;
                frames = 0;

                yield return new WaitForSeconds(frequency);
            }
        }
    }
}