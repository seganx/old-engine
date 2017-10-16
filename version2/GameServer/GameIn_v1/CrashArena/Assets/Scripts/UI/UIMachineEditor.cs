using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SeganX;

public class UIMachineEditor : Base
{
    public TextAsset jsonFile = null;

    public void Setup(Machine machine)
    {
        transform.RemoveChildrenBut(0);

        var bodyEditor = transform.GetChild<UIMachineEditorButton>(0);
        bodyEditor.Setup(machine.body.transform, Vector3.up * machine.body.Height);

        var slots = new List<Slot>();
        slots.AddRange(machine.body.wheels);
        slots.AddRange(machine.body.staffs);
        slots.AddRange(machine.body.weapons);

        slots.Sort((x, y) =>
        {
            var angleX = Vector3.SignedAngle(x.transform.localPosition - Vector3.up * 0.5f, Vector3.right, Vector3.back);
            var angleY = Vector3.SignedAngle(y.transform.localPosition - Vector3.up * 0.5f, Vector3.right, Vector3.back);
            return Mathf.RoundToInt(angleY - angleX);
        });

        foreach (var item in slots)
            bodyEditor.Clone<UIMachineEditorButton>(transform).Setup(item.transform, Vector3.zero);
    }

    // Use this for initialization
    void Start()
    {
        var root = Json.Parse(jsonFile.text);
        Debug.Log(root.ToString());

        var machine = Machine.Create();
        machine.Setup(root, MachineParty.Editor);
        machine.transform.localPosition = new Vector3(-3, -1, 0);

        Setup(machine);
    }

    // Update is called once per frame
    void Update()
    {

    }
}
