package org.wysaid.models;

import android.graphics.Bitmap;

/**
 * Created by niek on 12/04/2016.
 */
public class ImageFilterPreview {
    private Bitmap mPreviewImage;
    private int mDrawable;
    private String mTitle;
    private boolean mActive;

    public ImageFilterPreview(Bitmap previewImage, int drawable, String title) {
        mPreviewImage = previewImage;
        mDrawable = drawable;
        mTitle = title;
    }

    public Bitmap getPreviewImage() {
        return mPreviewImage;
    }

    public int getDrawable() {
        return mDrawable;
    }

    public String getTitle() {
        return mTitle;
    }

    public void setActive(boolean active) {
        mActive = active;
    }

    public boolean getActive() {
        return mActive;
    }
}
