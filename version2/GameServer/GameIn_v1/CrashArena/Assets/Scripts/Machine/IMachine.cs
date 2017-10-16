using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public enum MachineParty { Player, Opponent, Editor }

public interface IMachine
{
    MachineParty Party { get; set; }
    int SpriteOrder { get; set; }
    bool IsEditMode { get; }
    string SpriteLayerName { get; }

    void TakeDamage(float damage);
}
