using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UIPersianInputField : Base
{
    public bool autoAlignment = true;
    private InputField inputField = null;

    public Text inputText { get; private set; }

    void Awake()
    {
        inputField = GetComponent<InputField>();
        inputText = inputField.textComponent.gameObject.Clone<Text>();
        inputField.textComponent.color = new Color(0, 0, 0, 0);
        inputText.raycastTarget = false;
        if (inputField == null || inputText == null)
        {
            Debug.LogWarning("Can't find objects in " + name);
            enabled = false;
            return;
        }

        inputField.onValueChanged.AddListener(OnInputTextChanged);
    }

    void OnEnable()
    {
        OnInputTextChanged(inputField.text);
    }

    public void OnInputTextChanged(string value)
    {
        var res = inputField.text.Trim().CleanForPersian();
        inputText.SetTextAndWrap(res, autoAlignment);
    }
}
