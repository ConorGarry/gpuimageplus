package org.wysaid.listeners;

import android.graphics.PointF;

import org.wysaid.utils.FocusType;

/**
 * Created by niek on 28/11/2017.
 */
public interface OnFocusChangedListener {
    void onFocusPositionChanged(float moveX, float moveY);

    void onFocusRadiusChanged(float focusRadius);

    void onFocusTypeChanged(FocusType focusType);
}