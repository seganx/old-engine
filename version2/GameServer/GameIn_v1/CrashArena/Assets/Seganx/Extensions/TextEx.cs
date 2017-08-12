using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;


public static class TextEx
{
    static string WrapLine(this TextGenerator self, string line, TextGenerationSettings settings)
    {
        string str = "";

        float lineWidth = 0;
        float speceWidth = self.GetPreferredWidth(" ", settings);
        float maxWidth = (settings.generationExtents.x > 20 ? settings.generationExtents.x : 99999) * settings.scaleFactor;
        List<string> lines = new List<string>();

        var words = line.Split(' ');
        for (int i = words.Length - 1; i >= 0; --i)
        {
            lineWidth += self.GetPreferredWidth(words[i], settings) + speceWidth;
            if (lineWidth > maxWidth && str.Length > 0)
            {
                lineWidth = 0;
                lines.Add(str);
                str = "";
                i++;
            }
            else str = words[i] + " " + str;
        }
        if (str.Length > 0) lines.Add(str);

        return string.Join("\n", lines.ToArray());
    }

    public static Text SetTextAndWrap(this Text self, string text)
    {
        if (text.IsNullOrEmpty())
        {
            self.text = text;
            return self;
        }

        TextGenerationSettings settings = self.GetGenerationSettings(self.rectTransform.rect.size);
        TextGenerator generator = self.cachedTextGenerator;

        var lines = text.Split('\n');
        for (int i = 0; i < lines.Length; i++)
            lines[i] = generator.WrapLine(lines[i], settings);

        self.text = string.Join("\n", lines);
        return self;
    }

    public static float GetTextHeight(this Text self, string text)
    {
        if (text.IsNullOrEmpty())
        {
            self.text = text;
            return 0;
        }

        TextGenerationSettings settings = self.GetGenerationSettings(new Vector2(self.rectTransform.rect.width, self.rectTransform.rect.height));
        TextGenerator generator = self.cachedTextGenerator;
        return generator.GetPreferredHeight(text, settings) / settings.scaleFactor;
    }
}
