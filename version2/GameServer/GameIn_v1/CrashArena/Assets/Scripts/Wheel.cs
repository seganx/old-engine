using UnityEngine;
using System.Collections;
using SeganX;

public class Wheel : Entity
{
    public Wheel Setup(Json.Node root)
    {
        UpdateData(root);
        CheckSprite(Params.Wheel);
        gameObject.AddComponent<CircleCollider2D>();
        CheckRigidbody();

        var id = root[Params.Slot].AsInt;
        var slot = machine.body.wheels[id];

        if (machine.side != Side.Null)
        {
            var joint = machine.body.gameObject.AddComponent<WheelJoint2D>();
            joint.anchor = slot.transform.localPosition;
            joint.connectedBody = GetComponent<Rigidbody2D>();
            
            var suspension = joint.suspension;
            suspension.frequency = slot.frequency;
            joint.suspension = suspension;

            joint.useMotor = true;
            var motor = joint.motor;
            motor.motorSpeed = machine.side == Side.Opponent ? speed : -speed;
            joint.motor = motor;
        }

        transform.localPosition = slot.transform.localPosition;

        return this;
    }
}
