using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ListItemDialog : ListItemBase
{
    public Text nameLabel = null;
    public Image characterFace = null;
    public Text dialogText = null;

    public ListItemDialog Setup(Book.Dialog dialog)
    {
        data = dialog;

        nameLabel.SetTextAndWrap(dialog.name);
        dialogText.SetTextAndWrap(dialog.text, true);

        if (dialog.character.isEmpty)
            characterFace.sprite = gameManager.characterDefaults.Empty;
        else if (dialog.character.isPlayer)
            characterFace.sprite = gameManager.characterDefaults.Player;
        else
            characterFace.sprite = AssetCharacter.FindCharacterFace(dialog.character.family, dialog.character.face, gameManager.characterDefaults.Null);

        return this;
    }
}

