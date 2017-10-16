using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace SeganX
{
    public class GUIManager : Base
    {
        public delegate void Callback<T>(T obj) where T : Component;

        public string prefabPath = "";
        public GUIPage mainPage = null;

        private Canvas canvas = null;
        private GUIPage currentPage = null;
        private List<System.Type> pageStack = new List<System.Type>();
        private List<GUIPopup> popupStack = new List<GUIPopup>();

        public bool IsEmpty
        {
            get { return currentPage == null && CurrentPopup == null && pageStack.Count < 1; }
        }

        public GUIPopup CurrentPopup
        {
            get { return popupStack.Count > 0 ? popupStack[0] : null; }
        }

        // Use this for initialization
        void Start()
        {
            canvas = GetComponent<Canvas>();
            Resources.UnloadUnusedAssets();
        }

        // Update is called once per frame
        void Update()
        {
            //  handle escape key
            if (Input.GetKeyDown(KeyCode.Escape))// && game.door.IsOpen && !game.ShowLoader && network.IsConnected)
                BackButton();

            //  update ZOrder of pages and popups
            float z = 0;
            for (int i = 0; i < popupStack.Count; i++)
            {
                GUIPopup p = popupStack[i];
                p.ChangeZOrder(z);
                if (p.pushOthersDown)
                    z += 100;
            }

            if (currentPage != null)
                currentPage.ChangeZOrder(z);
        }

        private void AttachCanvas(GUIPanel panel)
        {
            if (panel == null) return;

            var panelcanvas = panel.GetComponent<Canvas>();
            if (panelcanvas == null)
                panel.transform.SetParent(transform, false);
            else if (panelcanvas.worldCamera == null)
                panelcanvas.worldCamera = canvas.worldCamera;
        }

        public T CurrentPage<T>() where T : GUIPage
        {
            return (currentPage is T) ? (currentPage as T) : null;
        }

        public T OpenPage<T>() where T : GUIPage
        {
            if (currentPage != null)
                Destroy(currentPage.gameObject, currentPage.Close());

            T page = Resources.Load<T>(prefabPath + typeof(T).Name);
            if (page == null)
            {
                Debug.LogError("GUIManager could not find page " + typeof(T).Name);
                return default(T);
            }
            pageStack.Insert(0, typeof(T));

            currentPage = Instantiate<GUIPage>(page);
            currentPage.name = page.name;
            AttachCanvas(currentPage);

            DelayCall(1, () => Resources.UnloadUnusedAssets());

            return currentPage as T;
        }

        public GUIPage ClosePage()
        {
            if (pageStack.Count > 1)
            {
                if (currentPage != null)
                    Destroy(currentPage.gameObject, currentPage.Close());
                pageStack.RemoveAt(0);

                var page = Resources.Load<GUIPage>(prefabPath + pageStack[0].Name);
                currentPage = Instantiate(page) as GUIPage;
                currentPage.name = page.name;
                AttachCanvas(currentPage);

                DelayCall(1, () => Resources.UnloadUnusedAssets());
            }

            return currentPage;
        }

        public T OpenPopup<T>(GameObject prefab) where T : GUIPopup
        {
            if (prefab == null)
                return null;
            T res = Instantiate<GameObject>(prefab).GetComponent<T>();
            popupStack.Insert(0, res);
            Resources.UnloadUnusedAssets();
            AttachCanvas(res);
            return res;
        }

        public T OpenPopup<T>() where T : GUIPopup
        {
            T popup = Resources.Load<T>(prefabPath + typeof(T).Name);
            if (popup == null)
            {
                Debug.LogError("GUIManager could not find popup " + typeof(T).Name);
                return null;
            }
            T res = Instantiate<T>(popup);
            popupStack.Insert(0, res);
            Resources.UnloadUnusedAssets();
            AttachCanvas(res);
            return res;
        }

        public int ClosePopup(GUIPopup popup)
        {
            popupStack.Remove(popup);
            Destroy(popup.gameObject);
            Resources.UnloadUnusedAssets();
            return popupStack.Count;
        }

        //  close current popup window and return the remains opened popup
        public int ClosePopup(bool closeAll = false)
        {
            if (popupStack.Count < 1) return 0;

            Destroy(popupStack[0].gameObject);
            popupStack.RemoveAt(0);
            Resources.UnloadUnusedAssets();

            return closeAll ? ClosePopup(closeAll) : popupStack.Count;
        }

        public GUIManager BackButton()
        {
            if (popupStack.Count > 0)
                popupStack[0].Back();
            else if (currentPage != null)
                currentPage.Back();
            return this;
        }

        public GUIManager CloseAll(bool closeDoor = true)
        {
            ClosePopup(true);
            BackButton();
            return this;
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

        //////////////////////////////////////////////////
        /// STATIC FUNCTION
        private static GUIManager instance;
        public static GUIManager Instance { get { if (instance == null) instance = FindObjectOfType<GUIManager>(); return instance; } }
        //////////////////////////////////////////////////
    }
}
