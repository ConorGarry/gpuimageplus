package org.wysaid.models;

import android.graphics.Bitmap;
import androidx.annotation.NonNull;

/**
 * Created by niek on 21/07/2017.
 */

public class FilterItemUi {
    private Bitmap mBitmapIcon;
    private int mDrawableIcon;
    private String mTitle;
    private boolean mActive;

    public FilterItemUi(@NonNull String title) {
        this(title, null, -1);
    }

    public FilterItemUi(@NonNull String title, Bitmap bitmapIcon) {
        this(title, bitmapIcon, -1);
    }

    public FilterItemUi(@NonNull String title, int drawableIcon) {
        this(title, null, drawableIcon);
    }

    private FilterItemUi(@NonNull String title, Bitmap bitmapIcon, int drawableIcon) {
        mTitle = title;
        mBitmapIcon = bitmapIcon;
        mDrawableIcon = drawableIcon;
    }

    public Bitmap getBitmapIcon() {
        return mBitmapIcon;
    }

    public void setBitmapIcon(Bitmap bitmapIcon) {
        mBitmapIcon = bitmapIcon;
    }

    public int getBitmapDrawable() {
        return mDrawableIcon;
    }

    public String getTitle() {
        return mTitle;
    }

    public boolean isActive() {
        return mActive;
    }
}
