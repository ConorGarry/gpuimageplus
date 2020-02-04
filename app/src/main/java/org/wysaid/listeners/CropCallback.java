package org.wysaid.listeners;

import android.graphics.Bitmap;
import android.graphics.RectF;

public interface CropCallback extends Callback {
    void onSuccess(Bitmap cropped, RectF cropRect);
    void onError();
}
