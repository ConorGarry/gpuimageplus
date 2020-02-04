package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/11/26.
 * Edited by niek
 */

// A wrapper for native class FrameRecorder
public class CGEFrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    private long mNativeAddress;

    public CGEFrameRenderer() {
        mNativeAddress = nativeCreateRenderer();
    }

    public boolean init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        return mNativeAddress != 0 && nativeInit(mNativeAddress, srcWidth, srcHeight, dstWidth, dstHeight);
    }

    //Will affect the framebuffer
    public void update(int externalTexture, float[] transformMatrix) {
        if (mNativeAddress != 0) nativeUpdate(mNativeAddress, externalTexture, transformMatrix);
    }

    public void runProc() {
        if (mNativeAddress != 0) nativeRunProc(mNativeAddress);
    }

    //Won't affect the framebuffer
    //the arguments means the viewport.
    public void render(int x, int y, int width, int height) {
        if (mNativeAddress != 0) nativeRender(mNativeAddress, x, y, width, height);
    }

    //set the flip/scaling for the camera texture
    public void setSrcFlipScale(float x, float y) {
        if (mNativeAddress != 0) nativeSetSrcFlipScale(mNativeAddress, x, y);
    }

    //set the render result's flip/scaling
    public void setRenderFlipScale(float x, float y) {
        if (mNativeAddress != 0) nativeSetRenderFlipScale(mNativeAddress, x, y);
    }

    //add a filter with config string
    public void addFilterWithConfig(final String config) {
        if (mNativeAddress != 0) nativeSetFilterWithConfig(mNativeAddress, config, false);
    }

    //set the value of the filter at index
    public void setFilterValueAtIndex(final String value, final int index) {
        if (mNativeAddress != 0) nativeSetFilterValueAtIndex(mNativeAddress, value, index);
    }

    /**
     * Set the size of the viewport
     *
     * @param renderWidth  New width of the viewport
     * @param renderHeight New height of the viewport
     */
    public void setRenderSize(final int renderWidth, final int renderHeight) {
        if (mNativeAddress != 0) nativeSetRenderSize(mNativeAddress, renderWidth, renderHeight);
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
     * Release this object's instance
     */
    public void release() {
        if (mNativeAddress != 0) {
            nativeRelease(mNativeAddress);
            mNativeAddress = 0;
        }
    }

    public void setMaskTexture(int texID, float aspectRatio) {
        if (mNativeAddress != 0) nativeSetMaskTexture(mNativeAddress, texID, aspectRatio);
    }

    /////////////////      protected         ///////////////////////

    protected native long nativeCreateRenderer();

    protected native boolean nativeInit(long holder, int srcWidth, int srcHeight, int dstWidth, int dstHeight);

    protected native void nativeUpdate(long holder, int externalTexture, float[] transformMatrix);

    protected native void nativeRunProc(long holder);

    protected native void nativeRender(long holder, int x, int y, int width, int height);

    protected native void nativeSetSrcFlipScale(long holder, float x, float y);

    protected native void nativeSetRenderFlipScale(long holder, float x, float y);

    protected native void nativeSetFilterWithConfig(long holder, String config, boolean shouldClearOlder);

    protected native void nativeSetFilterValueAtIndex(long holder, String value, int index);

    protected native void nativeSetMaskTexture(long holder, int texID, float aspectRatio);

    protected native void nativeRelease(long holder);

    protected native void nativeSetRenderSize(long holder, int renderWidth, int renderHeight);

    protected native void nativeSetSourceSize(long holder, int sourceWidth, int sourceHeight);
}
