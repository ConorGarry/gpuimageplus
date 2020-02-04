package org.wysaid.layouts;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.FrameLayout;

/**
 * Created by niek on 15/04/2016.
 */
public class TouchInterceptFrameLayout extends FrameLayout {

    private boolean mInterceptAllTouches = false;

    public TouchInterceptFrameLayout(Context context) {
        super(context);
    }

    public TouchInterceptFrameLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public TouchInterceptFrameLayout(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public void interceptAllTouches(boolean interceptAllTouches) {
        mInterceptAllTouches = interceptAllTouches;
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {
        return mInterceptAllTouches;
    }
}
