package org.wysaid.view;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Handler;
import android.os.Looper;
import androidx.annotation.AttrRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Size;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import org.wysaid.listeners.BitmapGeneratedListener;
import org.wysaid.listeners.FilterAdjustedListener;
import org.wysaid.listeners.OnFocusChangedListener;
import org.wysaid.listeners.ResponseListener;
import org.wysaid.models.FilterItem;
import org.wysaid.utils.FilterAdjustHelper;
import org.wysaid.utils.TextureRenderer;

import java.util.List;
import java.util.Locale;

/**
 * Created by niek on 27/06/2017.
 */

public class GPUImageView extends FrameLayout implements View.OnTouchListener {
    private static final int THUMBNAIL_GENERATION_DELAY = 50;

    private ImageGLSurfaceView mGLSurfaceView;
    private ImageView mIvOverlay;

    private Context mContext;

    private List<FilterItem> mThumbnailFilterItems;
    private BitmapGeneratedListener mBitmapGeneratedListener;

    protected int mCropXPos, mCropYPos, mCropWidth, mCropHeight, mBitmapWidth, mBitmapHeight;

    private Point mCropPoint;
    private Size mCropSize;

    private float mCropScale = 1.0f;

    public GPUImageView(@NonNull Context context) {
        this(context, null);
    }

    public GPUImageView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public GPUImageView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        mGLSurfaceView = new ImageGLSurfaceView(context, attrs);
        mIvOverlay = new ImageView(context, attrs);

        addView(mGLSurfaceView);
        addView(mIvOverlay);

        mNetFocusPoint = new PointF(0.5f, 0.5f);

        setOnTouchListener(this);
    }

    public void setBitmapAndFilters(final Bitmap bitmap, final List<FilterItem> filterItems,
                                    final Bitmap thumbnailBitmap, final List<FilterItem> thumbnailFilterItems,
                                    final BitmapGeneratedListener bitmapGeneratedListener, final Bitmap finalBitmap) {
        mThumbnailFilterItems = thumbnailFilterItems;
        mBitmapGeneratedListener = bitmapGeneratedListener;

        mBitmapWidth = bitmap.getWidth();
        mBitmapHeight = bitmap.getHeight();

        mIvOverlay.setImageBitmap(bitmap);

        mCropSize = new Size(bitmap.getWidth(), bitmap.getHeight());
        mCropPoint = new Point(0, 0);

        mGLSurfaceView.setSurfaceCreatedCallback(() -> {
            mGLSurfaceView.setRenderSize(mIvOverlay.getWidth(), mIvOverlay.getHeight());

            for (FilterItem filterItem : filterItems) {
                mGLSurfaceView.addFilterWithConfig(filterItem.getFilter());
            }

            mGLSurfaceView.setImageBitmap(bitmap, thumbnailBitmap, new ResponseListener() {
                @Override
                public void imageHasBeenSet() {
                    new Handler(Looper.getMainLooper()).postDelayed(() -> setViewVisibility(mIvOverlay, INVISIBLE), THUMBNAIL_GENERATION_DELAY);
                }

                @Override
                public void thumbnailImageHasBeenSet() {
                    processThumbnails(0, true);
                }
            });

            mGLSurfaceView.setFinalBitmap(finalBitmap);
        });
    }

    public void setFinalBitmapAndFilters(final @NonNull Bitmap bitmap) {
        if (mThumbnailFilterItems != null && mBitmapGeneratedListener != null) {
            mGLSurfaceView.setFinalBitmap(bitmap);
        }
    }

    public void refreshThumbnails(final Bitmap thumbnailBitmap) {
        if (mThumbnailFilterItems != null && mBitmapGeneratedListener != null) {
            mGLSurfaceView.setThumbnailBitmap(thumbnailBitmap, new ResponseListener() {
                @Override
                public void imageHasBeenSet() {
                    // won't be called
                }

                @Override
                public void thumbnailImageHasBeenSet() {
                    processThumbnails(0, false);
                }
            });
        }
    }

    private void processThumbnails(final int index, final boolean alsoDoBlurBitmap) {
        new Handler(Looper.getMainLooper()).postDelayed(() -> mGLSurfaceView.getThumbnailWithConfig(mThumbnailFilterItems.get(index).getFilter(), bitmap -> {
            mBitmapGeneratedListener.newThumbnailGenerated(index, mThumbnailFilterItems.get(index).getResource(), bitmap);
            if (index < mThumbnailFilterItems.size() - 1) {
                processThumbnails(index + 1, alsoDoBlurBitmap);
            } else if (alsoDoBlurBitmap) {
                mGLSurfaceView.getBlurBitmap(blurBitmap -> mBitmapGeneratedListener.blurBitmapGenerated(blurBitmap));
            }
        }), THUMBNAIL_GENERATION_DELAY);
    }

    public void setFilterValueAtIndex(final String value, final int index) {
        setFilterValueAtIndex(value, index, null, true);
    }

    public void setFilterValueAtIndex(final String value, final int index, boolean render) {
        setFilterValueAtIndex(value, index, null, render);
    }

    public void setFilterValueAtIndex(final String value, final int index, final FilterAdjustedListener filterAdjustedListener) {
        mGLSurfaceView.setFilterValueAtIndex(value, index, filterAdjustedListener, true);
    }

    public void setFilterValueAtIndex(final String value, final int index, final FilterAdjustedListener filterAdjustedListener, final boolean render) {
        mGLSurfaceView.setFilterValueAtIndex(value, index, filterAdjustedListener, render);
    }

    public void getCropBitmap(final int cropFilterIndex, final int vignetteFilterIndex, final int transformationFilterIndex, final ImageGLSurfaceView.QueryResultBitmapCallback callback) {
        mGLSurfaceView.getCropBitmap(
                cropFilterIndex, String.format(Locale.US, "%d %d %d %d", 0, 0, mBitmapWidth, mBitmapHeight),
                vignetteFilterIndex, "0.0 0.5 0.5 1.0 1.0",
                transformationFilterIndex, FilterAdjustHelper.getTransformationMatrix(0, 0),
                callback);
    }

    public void rotate90Degrees() {
        mGLSurfaceView.rotate90Degrees();
    }

    public String handleCropRotation() {
        mCropWidth = mCropWidth > 0 ? mCropWidth : mGLSurfaceView.getImageWidth();
        mCropHeight = mCropHeight > 0 ? mCropHeight : mGLSurfaceView.getImageHeight();

        int previousXPos = mCropXPos;
        mCropXPos = mGLSurfaceView.getImageHeight() - mCropYPos - mCropHeight;
        mCropYPos = previousXPos;

        int previousWidth = mCropWidth;
        mCropWidth = mCropHeight;
        mCropHeight = previousWidth;

        mCropSize = new Size(mCropWidth, mCropHeight);

        return String.format(Locale.US, "%1$d %2$d %3$d %4$d %5$d %6$d",
                mCropXPos, mCropYPos, mCropWidth, mCropHeight, mGLSurfaceView.getImageHeight(), mGLSurfaceView.getImageWidth());
    }

    public TextureRenderer.Viewport handleCrop(final String cropInfo, boolean swap) {
        String[] floats = cropInfo.split("\\s+");

        mCropXPos = Integer.parseInt(floats[0]);
        mCropYPos = Integer.parseInt(floats[1]);
        mCropWidth = Integer.parseInt(floats[2]);
        mCropHeight = Integer.parseInt(floats[3]);

        mCropPoint.set(mCropXPos, mCropYPos);
        mCropSize = new Size(mCropWidth, mCropHeight);

        return mGLSurfaceView.handleCrop(mCropWidth, mCropHeight, swap);
    }

    public void requestRender() {
        mGLSurfaceView.requestRender();
    }

    public void requestRender(boolean draw) {
        mGLSurfaceView.requestRender();
    }

    public RectF getOriginalViewport() {
        return mGLSurfaceView.getOriginalViewport();
    }

    public RectF getRotatedOriginalViewport() {
        return mGLSurfaceView.getRotatedOriginalViewport();
    }

    public RectF getViewport() {
        return mGLSurfaceView.getViewport();
    }

    public void setViewport(RectF viewportRect) {
        mGLSurfaceView.setViewport(viewportRect);
    }

    public void setViewport(int x, int y, int width, int height) {
        mGLSurfaceView.setViewport(x, y, width, height);
    }

    public void setViewportX(int viewportX) {
        mGLSurfaceView.setViewportX(viewportX);
    }

    public void setViewportY(int viewportY) {
        mGLSurfaceView.setViewportY(viewportY);
    }

    public void getResultBitmap(final ImageGLSurfaceView.QueryResultBitmapCallback callback) {
        mGLSurfaceView.getResultBitmap(callback);
    }

    public void getResultBitmap(final ImageGLSurfaceView.QueryResultBitmapCallback callback, final String[] configArray, final float outputScale, final int resultWidth, final int resultHeight) {
        mGLSurfaceView.getResultBitmap(callback, configArray, outputScale, resultWidth, resultHeight);
    }

    public String getFinalConfig(final String[] configArray) {
        return mGLSurfaceView.getFinalConfig(configArray);
    }

    public String getSavedFocusFilterConfig() {
        return mGLSurfaceView.getSavedFocusFilterConfig();
    }

    private void setViewVisibility(final View view, final int visibility) {
        ((Activity) mContext).runOnUiThread(() -> view.setVisibility(visibility));
    }

    public Point getCropPoint() {
        return mCropPoint;
    }

    public Size getCropSize() {
        return mCropSize;
    }

    public void release() {
        mGLSurfaceView.release();
    }

    //region touch events

    private static final float MIN_FOCUS_POINT_RADIUS = 50f;

    private boolean mFocusActive;

    private PointF mPointOne = new PointF(0.5f, 0.5f);
    private PointF mPointTwo = new PointF(0.5f, 0.5f);
    private PointF mNetFocusPoint;
    private float mInitialDistanceToPointOne;

    private int mPointerAmount;

    private PointF mDistancePoint = new PointF();

    private OnFocusChangedListener mOnFocusChangedListener;

    @Override
    public boolean onTouch(View v, MotionEvent motionEvent) {
        return mFocusActive ? handleFocusTouch(motionEvent) : handleNormalTouch(motionEvent);
    }

    private boolean handleNormalTouch(MotionEvent motionEvent) {
        switch (motionEvent.getAction()) {
            case MotionEvent.ACTION_DOWN:
                mIvOverlay.setVisibility(VISIBLE);
                mGLSurfaceView.setVisibility(INVISIBLE);
                return true;

            case MotionEvent.ACTION_UP:
                mIvOverlay.setVisibility(INVISIBLE);
                mGLSurfaceView.setVisibility(VISIBLE);
                return true;
        }

        return false;
    }

    private boolean handleFocusTouch(MotionEvent motionEvent) {
        switch (motionEvent.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_POINTER_DOWN:
            case MotionEvent.ACTION_DOWN:
                mPointerAmount++;

                if (mPointerAmount == 1) {
                    mPointOne.set(motionEvent.getX(), motionEvent.getY());
                } else if(mPointerAmount == 2) {
                    mPointOne.set(motionEvent.getX(0), motionEvent.getY(0));
                    mPointTwo.set(motionEvent.getX(1), motionEvent.getY(1));
                    mInitialDistanceToPointOne = distanceToPoint(mPointOne, mPointTwo);
                }
                break;

            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_UP:
                mPointerAmount--;

                if (mPointerAmount == 1) {
                    mPointOne.set(motionEvent.getX(), motionEvent.getY());
                } else if(mPointerAmount == 2) {
                    mPointOne.set(motionEvent.getX(0), motionEvent.getY(0));
                    mPointTwo.set(motionEvent.getX(1), motionEvent.getY(1));
                    mInitialDistanceToPointOne = distanceToPoint(mPointOne, mPointTwo);
                }
                break;

            case MotionEvent.ACTION_MOVE:
                if (mPointerAmount == 1) {
                    onFocusMove(motionEvent);
                } else if (mPointerAmount == 2) {
                    mPointOne.set(motionEvent.getX(0), motionEvent.getY(0));
                    mPointTwo.set(motionEvent.getX(1), motionEvent.getY(1));

                    onFocusZoom();
                }
                break;

            case MotionEvent.ACTION_CANCEL:
                break;
        }

        return true;
    }

    private void onFocusMove(MotionEvent motionEvent) {
        if (mOnFocusChangedListener != null) {
            float aspectX = (float) getWidth() / mGLSurfaceView.getViewport().right;
            float aspectY = (float) getHeight() / mGLSurfaceView.getViewport().bottom;

            float previousX = mPointOne.x / getWidth();
            float previousY = mPointOne.y / getHeight();
            float newX = motionEvent.getX() / getWidth();
            float newY = motionEvent.getY() / getHeight();

            mNetFocusPoint.set((newX - previousX), (newY - previousY));

            switch (FilterAdjustHelper.sAmountOf90DegreeRotations) {
                case 0:
                    mNetFocusPoint.set(mNetFocusPoint.x, mNetFocusPoint.y);
                    break;
                case 1:
                    mNetFocusPoint.set(mNetFocusPoint.y, mNetFocusPoint.x * -1);
                    break;
                case 2:
                    mNetFocusPoint.set(mNetFocusPoint.x * -1, mNetFocusPoint.y * -1);
                    break;
                case 3:
                    mNetFocusPoint.set(mNetFocusPoint.y * -1, mNetFocusPoint.x);
                    break;
                default:
                    mNetFocusPoint.set(mNetFocusPoint.x, mNetFocusPoint.y);
                    break;
            }

            mOnFocusChangedListener.onFocusPositionChanged(mNetFocusPoint.x, mNetFocusPoint.y);
            invalidate();

            mPointOne.set(motionEvent.getX(), motionEvent.getY());
        }
    }

    private void onFocusZoom() {
        float newDistance = distanceToPoint(mPointOne, mPointTwo);
        float toGain = newDistance - mInitialDistanceToPointOne;

        mInitialDistanceToPointOne = newDistance;

        if (mOnFocusChangedListener != null) {
            mOnFocusChangedListener.onFocusRadiusChanged(toGain / (float) (getWidth() - getPaddingLeft() - getPaddingRight()));

            invalidate();
        }
    }

    private float distanceToPoint(PointF pointerOne, PointF pointerTwo) {
        return distanceToPoint(pointerOne, pointerTwo.x, pointerTwo.y);
    }

    private float distanceToPoint(PointF pointerOne, float x2, float y2) {
        return (float) Math.sqrt(Math.pow((pointerOne.x - x2), 2) + Math.pow((pointerOne.y - y2), 2));
    }

    public void setOnFocusChangedListener(OnFocusChangedListener onFocusChangedListener) {
        mOnFocusChangedListener = onFocusChangedListener;
    }

    public void setFocusActive(boolean focusActive) {
        mFocusActive = focusActive;
    }

    public float resetCropScale() {
        return mCropScale = mGLSurfaceView.getCropScale();
    }

    //endregion
}
