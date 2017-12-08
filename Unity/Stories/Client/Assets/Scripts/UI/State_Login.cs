using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class State_Login : GameState
{
    public InputField username = null;
    public InputField password = null;
    public GameObject button = null;

    public override void Back()
    {
        Application.Quit();
    }


    public void OnLogin()
    {
        //gameManager.OpenState<State_BookList>();

        if (username.text.Length < 4 || password.text.Length < 4) return;
        button.SetActive(false);

    }
}
