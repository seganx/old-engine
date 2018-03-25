﻿using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace SeganX
{
    public class GameManager : Base
    {
        public string prefabPath = "Prefabs/UI/";
        public Canvas canvas = null;

        private GameState currentState = null;
        private List<System.Type> stateStack = new List<System.Type>();
        private List<GameState> guiStack = new List<GameState>();

        public GameState CurrentPopup { get { return guiStack.Count > 0 ? guiStack[0] : null; } }
        public GameState CurrentState { get { return currentState; } }
        public bool IsEmpty { get { return currentState == null && CurrentPopup == null && stateStack.Count < 1; } }

        public System.Action<GameState> OnBackButton = new System.Action<GameState>(x => { });
        public System.Action<GameState> OnOpenState = new System.Action<GameState>(x => { });

        public T OpenState<T>() where T : GameState
        {
            T state = Resources.Load<T>(prefabPath + typeof(T).Name);
            if (state == null)
            {
                Debug.LogError("GameManager could not find " + typeof(T).Name);
                return null;
            }

            if (currentState != null)
            {
                var delay = currentState.PreClose();
                Destroy(currentState.gameObject, delay);
                DelayCall(delay + 0.1f, () => Resources.UnloadUnusedAssets());
            }

            stateStack.Insert(0, typeof(T));
            currentState = Instantiate<GameState>(state);
            currentState.name = state.name;
            AttachState(currentState);

            OnOpenState(currentState);

            return currentState as T;
        }

        private GameState CloseState()
        {
            if (stateStack.Count < 2) return currentState;

            stateStack.RemoveAt(0);
            var delay = currentState.PreClose();
            Destroy(currentState.gameObject, delay);
            DelayCall(delay + 0.1f, () => Resources.UnloadUnusedAssets());

            var state = Resources.Load<GameState>(prefabPath + stateStack[0].Name);
            currentState = Instantiate(state) as GameState;
            currentState.name = state.name;
            AttachState(currentState);

            return currentState;
        }

        public T OpenPopup<T>(GameObject prefab) where T : GameState
        {
            if (prefab == null) return null;
            T res = Instantiate<GameObject>(prefab).GetComponent<T>();
            guiStack.Insert(0, res);
            Resources.UnloadUnusedAssets();
            AttachState(res);
            OnOpenState(res);
            return res;
        }

        public T OpenPopup<T>() where T : GameState
        {
            T popup = Resources.Load<T>(prefabPath + typeof(T).Name);
            if (popup == null)
            {
                Debug.LogError("GameManager could not find " + typeof(T).Name);
                return null;
            }
            return OpenPopup<T>(popup.gameObject);
        }

        public int ClosePopup(GameState popup)
        {
            guiStack.Remove(popup);
            var delay = popup.PreClose();
            Destroy(popup.gameObject, delay);
            DelayCall(delay + 0.1f, () => Resources.UnloadUnusedAssets());
            return guiStack.Count;
        }

        //  close current popup window and return the remains opened popup
        public int ClosePopup(bool closeAll = false)
        {
            if (guiStack.Count < 1) return 0;
            ClosePopup(guiStack[0]);
            return closeAll ? ClosePopup(closeAll) : guiStack.Count;
        }

        public GameManager CloseAll()
        {
            ClosePopup(true);
            BackButton();
            return this;
        }

        public GameManager BackButton()
        {
            OnBackButton(guiStack.Count > 0 ? guiStack[0] : currentState);

            if (guiStack.Count > 0)
                ClosePopup();
            else if (currentState != null)
                CloseState();
            return this;
        }

        private void AttachState(GameState panel)
        {
            if (panel == null) return;
            if (panel.transform is RectTransform)
            {
                var panelcanvas = panel.GetComponent<Canvas>();
                if (panelcanvas == null)
                    panel.transform.SetParent(canvas.transform, false);
                else if (panelcanvas.worldCamera == null)
                    panelcanvas.worldCamera = canvas.worldCamera;
            }
        }

        public virtual void LateUpdate()
        {
            //  handle escape key
            if (Input.GetKeyDown(KeyCode.Escape))
            {
                if (guiStack.Count > 0)
                    guiStack[0].Back();
                else if (currentState != null)
                    currentState.Back();
            }
        }

        private void OnValidate()
        {
            var validPath = Application.dataPath + "/Resources/" + prefabPath;
            if (System.IO.Directory.Exists(validPath) == false)
                System.IO.Directory.CreateDirectory(validPath);
        }

#if UNITY_EDITOR && OFF
        void OnGUI()
        {
            string str = "Popup Stack:\n";
            foreach (var item in popupStack)
                str += item.name + "\n";
            str += "Page Stack:\n";
            foreach (var item in pageStack)
                str += item.Name + "\n";
            GUI.Box(new Rect(0, 0, 100, 100), str);
        }
#endif


        ////////////////////////////////////////////////////////////
        /// STATIC MEMBERS
        ////////////////////////////////////////////////////////////
        private static GameManager instance = null;
        public static GameManager Instance { get { return instance == null ? (instance = FindObjectOfType<GameManager>()) : instance; } }
    }
}
