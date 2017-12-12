using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace SeganX
{
    public enum UIDragState { Normal, Dragging, Drop }

    public class UIDragable : Base, IDragHandler, IPointerDownHandler, IPointerUpHandler
    {
        public Rect bound;
        public Vector2 snapThreshold = Vector2.one;
        private Vector2 offset = Vector2.zero;

        public bool freezed { get; set; }
        public bool isDragging { get { return current == this; } }

        public Vector2 position { get; private set; }
        public Vector2 delta { get; private set; }

        public void OnPointerDown(PointerEventData eventData)
        {
            RectTransformUtility.ScreenPointToLocalPointInRectangle(parentRectTransform, eventData.position, eventData.enterEventCamera ?? eventData.pressEventCamera, out offset);
            offset = rectTransform.anchoredPosition - offset;
        }

        public void OnPointerUp(PointerEventData eventData)
        {
            cancelDrag = false;
            current = null;
        }

        public void OnDrag(PointerEventData eventData)
        {
            if (cancelDrag)
            {
                eventData.pointerDrag = null;
                cancelDrag = false;
                current = null;
                return;
            }
            if (freezed) return;
            current = this;

            Vector2 localPoint;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(parentRectTransform, eventData.position, eventData.enterEventCamera ?? eventData.pressEventCamera, out localPoint);
            localPoint += offset;
            localPoint.x = Mathf.RoundToInt(localPoint.x / snapThreshold.x) * snapThreshold.x;
            localPoint.y = Mathf.RoundToInt(localPoint.y / snapThreshold.y) * snapThreshold.y;

            if (localPoint.x < bound.x) localPoint.x = bound.x;
            if (localPoint.y > bound.y) localPoint.y = bound.y;
            if (localPoint.x > bound.x + bound.width - rectTransform.rect.width) localPoint.x = bound.x + bound.width - rectTransform.rect.width;
            if (localPoint.y < bound.y - bound.height + rectTransform.rect.height) localPoint.y = bound.y - bound.height + rectTransform.rect.height;

            position = localPoint;
            delta = eventData.delta;
        }


        ////////////////////////////////////////////////////////////
        /// STATIV MEMBERS
        ////////////////////////////////////////////////////////////
        public static UIDragable current = null;
        private static bool cancelDrag = false;
        public static void CancelDrag()
        {
            if (current)
                cancelDrag = true;
        }

    }

}

