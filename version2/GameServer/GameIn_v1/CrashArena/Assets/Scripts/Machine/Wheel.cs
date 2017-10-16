using UnityEngine;
using System.Collections;
using SeganX;

public class Wheel : Entity
{
    public Wheel Setup(Json.Node root, Body body)
    {
        UpdateData(root);
        AddSpriteRenderer(Params.Wheel);

        if (body == null) return this;

        AddColliderWithRigidBody<CircleCollider2D>();

        var id = root[Params.Slot].AsInt;
        var slot = body.wheels[id];
        transform.localPosition = slot.transform.localPosition;

        if (machine.IsEditMode) return this;

        var joint = body.gameObject.AddComponent<WheelJoint2D>();
        joint.anchor = slot.transform.localPosition;
        joint.connectedBody = GetComponent<Rigidbody2D>();

        var suspension = joint.suspension;
        suspension.frequency = slot.frequency;
        joint.suspension = suspension;

        joint.useMotor = true;
        var motor = joint.motor;
        motor.motorSpeed = machine.Party == MachineParty.Opponent ? speed : -speed;
        joint.motor = motor;


        return this;
    }
}
