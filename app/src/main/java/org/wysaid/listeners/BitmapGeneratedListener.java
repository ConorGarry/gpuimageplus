package org.wysaid.listeners;

import android.graphics.Bitmap;

/**
 * Created by niek on 27/06/2017.
 */

public interface BitmapGeneratedListener {
    void newThumbnailGenerated(int listIndex, String resource, Bitmap bitmap);
    void blurBitmapGenerated(Bitmap blurBitmap);
}