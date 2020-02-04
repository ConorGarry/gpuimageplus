package org.wysaid.nativePort;

import android.graphics.Bitmap;
import androidx.annotation.NonNull;
import android.util.Log;

/**
 * Created by wysaid on 15/12/25.
 * Edited by niek
 */

// C++ native class 'CGEImageHandlerAndroid'

public class CGEImageHandler {

    static {
        NativeLibraryLoader.load();
    }

    private long mNativeAddress;

    public CGEImageHandler() {
        mNativeAddress = nativeCreateHandler();
        Log.e("", "native handler address is: " + mNativeAddress);
    }

    public boolean initWithBitmap(@NonNull Bitmap bmp) {
        if (bmp.getConfig() != Bitmap.Config.ARGB_8888) {
            bmp = bmp.copy(Bitmap.Config.ARGB_8888, false);
        }

        return nativeInitWithBitmap(mNativeAddress, bmp);
    }

    public Bitmap getResultBitmap() {
        return nativeGetResultBitmap(mNativeAddress);
    }

    public Bitmap getResultBitmap(final String config, final float outputScale, final int resultWidth, final int resultHeight) {
        return nativeGetResultBitmapWithResultSize(mNativeAddress, config, outputScale, resultWidth, resultHeight);
    }

    public Bitmap getBlurBitmap() {
        return nativeGetBlurBitmap(mNativeAddress);
    }

    public void setDrawerFlipScale(float x, float y) {
        nativeSetDrawerFlipScale(mNativeAddress, x, y);
    }

    /**
     * @param config           The filter rule string. Pass null for config to clear all filters.
     * @param shouldClearOlder The last filter will be cleared if it's true.
     *                         There may be memory leaks if you pass false for 'shouldClearOlder'
     *                         when you don't clear it by yourself.
     * @param shouldProcess    This marks if the filter should be run right now.
     *                         The result will not change if you pass false for 'shouldProcess'
     *                         until you call 'processingFilters'.
     */
    public void setFilterWithConfig(String config, boolean shouldClearOlder, boolean shouldProcess) {
        nativeSetFilterWithConfig(mNativeAddress, config, shouldClearOlder, shouldProcess);
    }

    /**
     * @param value         filter value
     * @param index         Only the intensity of the filter on the given index will be changed.
     * @param shouldProcess This marks if the filter should be run right now.
     *                      The result will not change if you pass false for 'shouldProcess'
     *                      until you call 'processingFilters'.
     * @return marks if this function worked. (It will fail when the index is out of range.)
     */
    public boolean setFilterValueAtIndex(String value, int index, boolean shouldProcess) {
        return nativeSetFilterValueAtIndex(mNativeAddress, value, index, shouldProcess);
    }

    /**
     * Set the size of the viewport
     *
     * @param renderWidth  New width of the viewport
     * @param renderHeight New height of the viewport
     */
    public void setRenderSize(int renderWidth, int renderHeight) {
        nativeSetRenderSize(mNativeAddress, renderWidth, renderHeight);
    }

    /**
     * Set the size of the source object (either video or image)
     *
     * @param sourceWidth  New width of the source
     * @param sourceHeight New height of the source
     */
    public void setSourceSize(int sourceWidth, int sourceHeight) {
        nativeSetSourceSize(mNativeAddress, sourceWidth, sourceHeight);
    }

    /**
     * Set the scale multiplier of the output bitmap
     *
     * @param outputScale New output scale
     */
    public void setOutputScale(float outputScale) {
        nativeSetOutputScale(mNativeAddress, outputScale);
    }

    public Bitmap getThumbnailWithConfig(String config) {
        return nativeGetThumbnailWithConfig(mNativeAddress, config);
    }

    public int getFocusType() {
        return nativeGetFocusType(mNativeAddress);
    }

    public float getFocusXPosition() {
        return nativeGetFocusXPosition(mNativeAddress);
    }

    public float getFocusYPosition() {
        return nativeGetFocusYPosition(mNativeAddress);
    }

    public float getFocusRadius() {
        return nativeGetFocusRadius(mNativeAddress);
    }

    public float getCropScale() {return nativeGetCropScale(mNativeAddress);}

    public void drawResult() {
        nativeDrawResult(mNativeAddress);
    }

    /**
     * Release this object's instance
     */
    public void release() {
        if (mNativeAddress != 0) {
            nativeRelease(mNativeAddress);
            mNativeAddress = 0;
        }
    }

    /////////////////      protected         ///////////////////////

    protected native long nativeCreateHandler();

    protected native boolean nativeInitWithBitmap(long holder, Bitmap bmp);

    protected native Bitmap nativeGetResultBitmap(long holder);

    protected native Bitmap nativeGetResultBitmapWithResultSize(long holder, String config, float outputScale, int resultWidth, int resultHeight);

    protected native Bitmap nativeGetBlurBitmap(long holder);

    protected native Bitmap nativeGetThumbnailWithConfig(long holder, String config);

    protected native void nativeSetDrawerFlipScale(long holder, float x, float y);

    protected native boolean nativeSetFilterWithConfig(long holder, String config, boolean shouldCleanOlder, boolean shouldProcess);

    protected native boolean nativeSetFilterValueAtIndex(long holder, String value, int index, boolean shouldProcess);

    protected native void nativeDrawResult(long holder);

    protected native void nativeRelease(long holder);

    protected native void nativeSetRenderSize(long holder, int renderWidth, int renderHeight);

    protected native void nativeSetSourceSize(long holder, int sourceWidth, int sourceHeight);

    protected native void nativeSetOutputScale(long holder, float outputScale);

    protected native int nativeGetFocusType(long holder);

    protected native float nativeGetFocusXPosition(long holder);

    protected native float nativeGetFocusYPosition(long holder);

    protected native float nativeGetFocusRadius(long holder);

    protected native float nativeGetCropScale(long holder);
}
