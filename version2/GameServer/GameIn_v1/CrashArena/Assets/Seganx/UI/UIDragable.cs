using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace SeganX
{

    public class UIDragable : Base, IDragHandler, IPointerDownHandler, IPointerUpHandler
    {
        public bool isDraging { get; private set; }

        private Vector2 offset = Vector2.zero;

        public void OnPointerDown(PointerEventData eventData)
        {
            RectTransformUtility.ScreenPointToLocalPointInRectangle(parentRectTransform, eventData.position, eventData.enterEventCamera ?? eventData.pressEventCamera, out offset);
            offset = rectTransform.anchoredPosition - offset;
        }

        public void OnPointerUp(PointerEventData eventData)
        {
            isDraging = false;
        }

        public virtual void OnDrag(PointerEventData eventData)
        {
            isDraging = true;

            Vector2 localPoint;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(parentRectTransform, eventData.position, eventData.enterEventCamera ?? eventData.pressEventCamera, out localPoint);
            rectTransform.anchoredPosition = localPoint + offset;
        }
    }

}

