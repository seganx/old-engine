using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }

    //////////////////////////////////////////////////
    /// STATIC FUNCTION
    private static GameManager instance;
    public static GameManager Instance { get { if (instance == null) instance = FindObjectOfType<GameManager>(); return instance; } }
    //////////////////////////////////////////////////
}
