package org.wysaid.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.common.Common;
import org.wysaid.listeners.FilterAdjustedListener;
import org.wysaid.listeners.ResponseListener;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.utils.FocusType;
import org.wysaid.utils.TextureRenderer;

import java.util.Locale;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wysaid on 15/12/23.
 * Edited by niek
 */
public class ImageGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer {

    public static final String LOG_TAG = Common.LOG_TAG;

    public enum DisplayMode {
        DISPLAY_SCALE_TO_FILL,
        DISPLAY_ASPECT_FILL,
        DISPLAY_ASPECT_FIT,
    }

    protected CGEImageHandler mImageHandler, mImageHandlerThumbs, mImageHandlerFinal;

    public CGEImageHandler getImageHandler() {
        return mImageHandler;
    }

    public CGEImageHandler getImageHandlerThumbnails() {
        return mImageHandlerThumbs;
    }

    public CGEImageHandler getImageHandlerFinal() {
        return mImageHandlerFinal;
    }

    protected TextureRenderer.Viewport mRenderViewport = new TextureRenderer.Viewport();
    protected int mImageWidth, mImageHeight;
    protected int mImageWidthCropped, mImageHeightCropped;
    protected int mViewWidth, mViewHeight;

    private int mOriginalRenderX, mOriginalRenderY, mOriginalRenderWidth, mOriginalRenderHeight;
    private int mRotatedOriginalRenderX, mRotatedOriginalRenderY, mRotatedOriginalRenderWidth, mRotatedOriginalRenderHeight;

    public int getImageWidth() {
        return mImageWidth;
    }

    public int getImageHeight() {
        return mImageHeight;
    }

    protected DisplayMode mDisplayMode = DisplayMode.DISPLAY_ASPECT_FIT;

    public DisplayMode getDisplayMode() {
        return mDisplayMode;
    }

    public void setDisplayMode(DisplayMode displayMode) {
        mDisplayMode = displayMode;
        calcViewport(false, true);
        requestRender();
    }

    public void addFilterWithConfig(final String config) {
        if (mImageHandler == null) return;

        queueEvent(() -> {
            if (mImageHandler == null) {
                Log.e(LOG_TAG, "set config after release!!");

                return;
            }
            mImageHandler.setFilterWithConfig(config, false, true);
            requestRender();
        });
    }

    public void getThumbnailWithConfig(final String config, final QueryResultBitmapCallback callback) {
        if (mImageHandlerThumbs == null) return;

        queueEvent(() -> {
            if (mImageHandlerThumbs == null) {
                Log.e(LOG_TAG, "set config after release!!");
                return;
            }

            Bitmap bitmap = mImageHandlerThumbs.getThumbnailWithConfig(config);
            callback.get(bitmap);
        });
    }

    protected final Object mSettingIntensityLock = new Object();
    protected int mSettingIntensityCount = 1;

    //See: CGEImageHandler.setFilterValueAtIndex
    public void setFilterValueAtIndex(final String value, final int index, final FilterAdjustedListener filterAdjustedListener) {
        setFilterValueAtIndex(value, index, filterAdjustedListener, true);
    }

    //See: CGEImageHandler.setFilterValueAtIndex
    public void setFilterValueAtIndex(final String value, final int index, final FilterAdjustedListener filterAdjustedListener, final boolean render) {
        if (mImageHandler == null) return;

        synchronized (mSettingIntensityLock) {

            if (mSettingIntensityCount <= 0) {
                Log.i(LOG_TAG, "Strength adjustment too fast, discard update frame...");
                return;
            }
            --mSettingIntensityCount;
        }

        queueEvent(() -> {
            if (mImageHandler == null) {
                Log.e(LOG_TAG, "set value after release!!");
            } else {
                mImageHandler.setFilterValueAtIndex(value, index, true);
                if (render) requestRender();

                if (filterAdjustedListener != null) {
                    filterAdjustedListener.filterHasBeenAdjusted();
                }
            }

            synchronized (mSettingIntensityLock) {
                ++mSettingIntensityCount;
            }
        });
    }

    public void getCropBitmap(final int cropFilterIndex, final String cropFilterValue,
                              final int vignetteFilterIndex, final String vignetteFilterValue,
                              final int transformationFilterIndex, final String transformationFilterValue,
                              final QueryResultBitmapCallback callback) {
        if (callback == null) return;

        queueEvent(() -> {
            mImageHandler.setFilterValueAtIndex(cropFilterValue, cropFilterIndex, true);
            mImageHandler.setFilterValueAtIndex(vignetteFilterValue, vignetteFilterIndex, true);
            mImageHandler.setFilterValueAtIndex(transformationFilterValue, transformationFilterIndex, true);
            callback.get(mImageHandler.getResultBitmap());
        });
    }

    public void setImageBitmap(final Bitmap bitmap, final Bitmap thumbnailBitmap, final ResponseListener responseListener) {
        if (bitmap == null || thumbnailBitmap == null) return;

        if (mImageHandler == null || mImageHandlerThumbs == null) {
            Log.e(LOG_TAG, "Handler not initialized!");
            return;
        }

        mImageWidth = bitmap.getWidth();
        mImageHeight = bitmap.getHeight();

        queueEvent(() -> {
            if (mImageHandler == null) {
                Log.e(LOG_TAG, "set render size after release!!");

                return;
            }
            mImageHandler.setSourceSize(mImageWidth, mImageHeight);
        });

        queueEvent(() -> {
            if (mImageHandler == null) {
                Log.e(LOG_TAG, "set image after release!!");
                return;
            }

            if (mImageHandler.initWithBitmap(bitmap)) {

                calcViewport(true, true);

                mRotatedOriginalRenderX = mRenderViewport.x;
                mRotatedOriginalRenderY = mRenderViewport.y;
                mRotatedOriginalRenderWidth = mRenderViewport.width;
                mRotatedOriginalRenderHeight = mRenderViewport.height;

                calcViewport(false, true);

                mOriginalRenderX = mRenderViewport.x;
                mOriginalRenderY = mRenderViewport.y;
                mOriginalRenderWidth = mRenderViewport.width;
                mOriginalRenderHeight = mRenderViewport.height;

                requestRender();

                responseListener.imageHasBeenSet();

                queueEvent(() -> {
                    if (mImageHandlerThumbs == null) {
                        Log.e(LOG_TAG, "set image after release!!");
                        return;
                    }

                    if (mImageHandlerThumbs.initWithBitmap(thumbnailBitmap)) {
                        responseListener.thumbnailImageHasBeenSet();
                    } else {
                        Log.e(LOG_TAG, "setImageBitmap: 初始化 handler 失败!");
                    }
                });

            } else {
                Log.e(LOG_TAG, "setImageBitmap: 初始化 handler 失败!");
            }
        });
    }

    public void setFinalBitmap(final Bitmap bitmap) {
        queueEvent(() -> {
            if (mImageHandlerFinal == null) {
                Log.e(LOG_TAG, "image handle final is null");
                return;
            }

            if (!mImageHandlerFinal.initWithBitmap(bitmap)) {
                Log.e(LOG_TAG, "setImageBitmap: 初始化 handler 失败!");
            }
        });
    }

    public void setThumbnailBitmap(final Bitmap thumbnailBitmap, final ResponseListener responseListener) {
        queueEvent(() -> {
            if (mImageHandlerThumbs == null) {
                Log.e(LOG_TAG, "set thumbnail image after release!!");
                return;
            }

            if (mImageHandlerThumbs.initWithBitmap(thumbnailBitmap)) {
                responseListener.thumbnailImageHasBeenSet();
            } else {
                Log.e(LOG_TAG, "setThumbnailBitmap: failed");
            }
        });
    }

    public void rotate90Degrees() {
        int previousImageWidth = mImageWidth;
        mImageWidth = mImageHeight;
        mImageHeight = previousImageWidth;

        int previousCropWidth = mImageWidthCropped;
        mImageWidthCropped = mImageHeightCropped;
        mImageHeightCropped = previousCropWidth;

        calcViewport(false, true);
    }

    public interface QueryResultBitmapCallback {
        void get(Bitmap bitmap);
    }

    public void getResultBitmap(final QueryResultBitmapCallback callback) {
        if (callback == null) return;

        queueEvent(() -> {
            Bitmap bitmap = mImageHandler.getResultBitmap();
            callback.get(bitmap);
        });
    }

    public void getResultBitmap(final QueryResultBitmapCallback callback, final String[] configArray, final float outputScale, final int resultWidth, final int resultHeight) {
        if (callback != null) queueEvent(() -> callback.get(mImageHandlerFinal.getResultBitmap(getFinalConfig(configArray), outputScale, resultWidth, resultHeight)));

    }

    public String getFinalConfig(final String[] configArray) {
        final String FOCUS = "@adjust focus";

        StringBuilder configBuilder = new StringBuilder();
        for (String filter : configArray) {
            filter = filter.replaceAll("( +)", " ").trim();

            // focus filter
            if (filter.contains(String.format(Locale.US, "%s", FOCUS))) {
                if (mImageHandler.getFocusType() == FocusType.RADIAL.getValue() || mImageHandler.getFocusType() == FocusType.LINEAR.getValue()) {
                    Bitmap blurBitmap = mImageHandlerFinal.getBlurBitmap();
                    CGENativeLibrary.TextureResult textureResult = CGENativeLibrary.loadTextureByBitmap(blurBitmap);

                    filter = filter.replace(FOCUS, String.format(Locale.US, "%s %s %s",
                            FOCUS,
                            textureResult.toFilterConfig(),
                            getSavedFocusFilterConfig()));
                }
            }
            configBuilder.append(String.format(Locale.US, "%s ", filter));
        }

        return (configBuilder.toString());
    }

    public String getSavedFocusFilterConfig() {
        return String.format(Locale.US, "%d %f %f %f",
                mImageHandler.getFocusType(),
                mImageHandler.getFocusXPosition(),
                mImageHandler.getFocusYPosition(),
                mImageHandler.getFocusRadius());
    }

    public void getBlurBitmap(final QueryResultBitmapCallback callback) {
        if (callback != null) queueEvent(() -> callback.get(mImageHandler.getBlurBitmap()));
    }

    public void getThumbnailBitmap(final QueryResultBitmapCallback callback) {
        if (callback != null) queueEvent(() -> callback.get(mImageHandlerThumbs.getResultBitmap()));
    }

    public void setRenderSize(final int renderWidth, final int renderHeight) {
        if (mImageHandler == null) return;

        queueEvent(() -> {
            if (mImageHandler == null) {
                Log.e(LOG_TAG, "set render size after release!!");
                return;
            }

            mImageHandler.setRenderSize(renderWidth, renderHeight);
        });
    }

    public ImageGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        setZOrderOnTop(true);

        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.TRANSLUCENT);

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);

        Log.i(LOG_TAG, "ImageGLTextureView Construct...");
    }

    public interface OnSurfaceCreatedCallback {
        void surfaceCreated();
    }

    protected OnSurfaceCreatedCallback mSurfaceCreatedCallback;

    public void setSurfaceCreatedCallback(OnSurfaceCreatedCallback callback) {
        mSurfaceCreatedCallback = callback;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "ImageGLTextureView onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);

        mImageHandlerFinal = new CGEImageHandler();
        mImageHandlerFinal.setDrawerFlipScale(1.0f, -1.0f);

        mImageHandlerThumbs = new CGEImageHandler();
        mImageHandlerThumbs.setDrawerFlipScale(1.0f, -1.0f);

        mImageHandler = new CGEImageHandler();
        mImageHandler.setDrawerFlipScale(1.0f, -1.0f);

        if (mSurfaceCreatedCallback != null) {
            mSurfaceCreatedCallback.surfaceCreated();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        mViewWidth = width;
        mViewHeight = height;
        calcViewport(false, true);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        if (mImageHandler == null) return;

        GLES20.glViewport(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height);
        mImageHandler.drawResult();
    }

    public void release() {
        if (mImageHandler != null || mImageHandlerThumbs != null || mImageHandlerFinal != null) {
            queueEvent(() -> {
                Log.i(LOG_TAG, "ImageGLTextureView release...");

                if (mImageHandler != null) {
                    mImageHandler.release();
                    mImageHandler = null;
                }

                if (mImageHandlerThumbs != null) {
                    mImageHandlerThumbs.release();
                    mImageHandlerThumbs = null;
                }

                if (mImageHandlerFinal != null) {
                    mImageHandlerFinal.release();
                    mImageHandlerFinal = null;
                }
            });
        }
    }

    public void setSourceSize(int width, int height) {
        mImageHandler.setSourceSize(width, height);
    }

    public RectF getOriginalViewport() {
        return new RectF(mOriginalRenderX, mOriginalRenderY, mOriginalRenderWidth, mOriginalRenderHeight);
    }

    public RectF getRotatedOriginalViewport() {
        return new RectF(mRotatedOriginalRenderX, mRotatedOriginalRenderY, mRotatedOriginalRenderWidth, mRotatedOriginalRenderHeight);
    }

    public RectF getViewport() {
        return new RectF(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height);
    }

    public void setViewport(RectF viewportRect) {
        setViewport(Math.round(viewportRect.left), Math.round(viewportRect.top), Math.round(viewportRect.right), Math.round(viewportRect.bottom));
    }

    public void setViewport(int x, int y, int width, int height) {
        mRenderViewport.x = x;
        mRenderViewport.y = y;
        mRenderViewport.width = width;
        mRenderViewport.height = height;
    }

    public void setViewportX(int viewportX) {
        mRenderViewport.x = viewportX;
    }

    public void setViewportY(int viewportY) {
        mRenderViewport.y = viewportY;
    }

    public TextureRenderer.Viewport handleCrop(float width, float height, boolean swap) {
        mImageWidthCropped = !swap ? (int) width : (int) height;
        mImageHeightCropped = !swap ? (int) height : (int) width;

        return calcViewport(false, false);
    }

    protected TextureRenderer.Viewport calcViewport(boolean rotated, boolean setViewport) {
        int width = !rotated ?
                mImageWidthCropped > 0 ? mImageWidthCropped : mImageWidth :
                mImageHeightCropped > 0 ? mImageHeightCropped : mImageHeight;
        int height = !rotated ?
                mImageHeightCropped > 0 ? mImageHeightCropped : mImageHeight :
                mImageWidthCropped > 0 ? mImageWidthCropped : mImageWidth;

        return calcViewport(width, height, setViewport);
    }

    protected TextureRenderer.Viewport calcViewport(int imageWidth, int imageHeight, boolean setViewport) {
        if (mDisplayMode == DisplayMode.DISPLAY_SCALE_TO_FILL) {
            if (setViewport) {
                mRenderViewport.x = 0;
                mRenderViewport.y = 0;
                mRenderViewport.width = mViewWidth;
                mRenderViewport.height = mViewHeight;

                return new TextureRenderer.Viewport(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height);
            } else {
                return new TextureRenderer.Viewport(0, 0, mViewWidth, mViewHeight);
            }
        }

        float scaling;

        scaling = imageWidth / (float) imageHeight;

        float viewRatio = mViewWidth / (float) mViewHeight;
        float s = scaling / viewRatio;

        int w, h;

        switch (mDisplayMode) {
            case DISPLAY_ASPECT_FILL: {
                if (s > 1.0) {
                    w = (int) (mViewHeight * scaling);
                    h = mViewHeight;
                } else {
                    w = mViewWidth;
                    h = (int) (mViewWidth / scaling);
                }
            }
            break;
            case DISPLAY_ASPECT_FIT: {
                if (s < 1.0) {
                    w = (int) (mViewHeight * scaling);
                    h = mViewHeight;
                } else {
                    w = mViewWidth;
                    h = (int) (mViewWidth / scaling);
                }
            }
            break;

            default:
                Log.i(LOG_TAG, "Error occured, please check the code...");
                return null;
        }

        if (setViewport) {
            mRenderViewport.width = w;
            mRenderViewport.height = h;
            mRenderViewport.x = (mViewWidth - w) / 2;
            mRenderViewport.y = (mViewHeight - h) / 2;
        }

        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height));

        return new TextureRenderer.Viewport((mViewWidth - w) / 2, (mViewHeight - h) / 2, w, h);
    }

    public float getCropScale() {
        return mImageHandler.getCropScale();
    }
}
