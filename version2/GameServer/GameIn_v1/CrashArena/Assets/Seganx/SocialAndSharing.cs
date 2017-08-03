using UnityEngine;
using System.Collections;

public class SocialAndSharing
{
    //public static string SocialLink
    //{
    //    get { return PlayerPrefsEx.GetString("SocialAndSharing.SocialLink", ""); }
    //    set { PlayerPrefs.SetString("SocialAndSharing.SocialLink", value); }
    //}

    public static string FacebookLink
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.FacebookLink", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.FacebookLink", value); }
    }

    public static string InstagramLink
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.InstagramLink", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.InstagramLink", value); }
    }

    public static string TelegramLink
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.TelegramLink", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.TelegramLink", value); }
    }

    public static string FeedbackEmail
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.FeedbackEmail", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.FeedbackEmail", value); }
    }

    public static string RateUsLink
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.RateUsLink", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.RateUsLink", value); }
    }

    public static string MarketPackage
    {
        get { return PlayerPrefsEx.GetString("SocialAndSharing.MarketPackage", ""); }
        set { PlayerPrefsEx.SetString("SocialAndSharing.MarketPackage", value); }
    }

#if UNITY_ANDROID
    private class IntentClass
    {
        public AndroidJavaClass iClass = null;
        public AndroidJavaObject iObject = null;
        public AndroidJavaObject currentActivity = null;

        public IntentClass()
        {
            iClass = new AndroidJavaClass("android.content.Intent");
            iObject = new AndroidJavaObject("android.content.Intent");
            AndroidJavaClass unity = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
            currentActivity = unity.GetStatic<AndroidJavaObject>("currentActivity");
        }

        public IntentClass SetText(string message, string title = "")
        {
            iObject.Call<AndroidJavaObject>("setAction", iClass.GetStatic<string>("ACTION_SEND"));
            if (string.IsNullOrEmpty(title) == false)
                iObject.Call<AndroidJavaObject>("putExtra", iClass.GetStatic<string>("EXTRA_TITLE"), title);
            iObject.Call<AndroidJavaObject>("putExtra", iClass.GetStatic<string>("EXTRA_TEXT"), message);
            iObject.Call<AndroidJavaObject>("setType", "text/plain");
            return this;
        }

        public IntentClass SetImage(string imagePath)
        {
            AndroidJavaClass uriClass = new AndroidJavaClass("android.net.Uri");
            iObject.Call<AndroidJavaObject>("setAction", iClass.GetStatic<string>("ACTION_SEND"));
            AndroidJavaObject uriObject = uriClass.CallStatic<AndroidJavaObject>("parse", "file://" + imagePath);
            iObject.Call<AndroidJavaObject>("putExtra", iClass.GetStatic<string>("EXTRA_STREAM"), uriObject);
            iObject.Call<AndroidJavaObject>("setType", "image/png");
            return this;
        }

        public IntentClass SetRateUs(string marketPackage, string uri)
        {
            AndroidJavaClass uriClass = new AndroidJavaClass("android.net.Uri");
            iObject.Call<AndroidJavaObject>("setAction", iClass.GetStatic<string>("ACTION_EDIT"));
            iObject.Call<AndroidJavaObject>("setData", uriClass.CallStatic<AndroidJavaObject>("parse", uri));
            if (marketPackage.Contains("."))
                iObject.Call<AndroidJavaObject>("setPackage", marketPackage);
            return this;
        }

        public IntentClass SetView(string packName, string uri)
        {
            AndroidJavaClass uriClass = new AndroidJavaClass("android.net.Uri");
            iObject.Call<AndroidJavaObject>("setAction", iClass.GetStatic<string>("ACTION_VIEW"));
            iObject.Call<AndroidJavaObject>("setData", uriClass.CallStatic<AndroidJavaObject>("parse", uri));
            if (packName.Contains("."))
                iObject.Call<AndroidJavaObject>("setPackage", packName);
            return this;
        }

        public IntentClass Start()
        {
            currentActivity.Call("startActivity", iObject);
            return this;
        }
    }
#endif

    public static void ShareText(string message, string title = "")
    {
#if UNITY_ANDROID
        new IntentClass().SetText(message, title).Start();
#endif
    }

    public static void RateUs()
    {
#if UNITY_ANDROID
        if (MarketPackage.IsNullOrEmpty())
            Application.OpenURL(RateUsLink);
        else if (MarketPackage == "view")
            new IntentClass().SetView(MarketPackage, RateUsLink).Start();
        else
            new IntentClass().SetRateUs(MarketPackage, RateUsLink).Start();
#endif
    }

    public static void OpenTelegram(string username)
    {
        Application.OpenURL("https://telegram.me/" + username);
    }

    public static void OpenTelegram()
    {
        Application.OpenURL(TelegramLink);
    }

    public static void OpenInstagram()
    {
        Application.OpenURL(InstagramLink);
    }

    public static void OpenFacebook()
    {
        Application.OpenURL(FacebookLink);
    }

    public static void SendEmail(string subject, string body)
    {
        if (FeedbackEmail.Length > 5)
            Application.OpenURL(FeedbackEmail);
        if (FeedbackEmail.Length < 5)
            return;
        Application.OpenURL("mailto:" + FeedbackEmail + "?subject=" + subject.EscapeURL() + "&body=" + body.EscapeURL());
    }
}
