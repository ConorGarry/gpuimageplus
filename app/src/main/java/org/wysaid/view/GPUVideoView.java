package org.wysaid.view;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import androidx.annotation.AttrRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.FrameLayout;
import android.widget.ImageView;

import org.wysaid.library.R;
import org.wysaid.listeners.FrameRendererCreatedListener;
import org.wysaid.listeners.BitmapGeneratedListener;
import org.wysaid.listeners.VideoControlListener;
import org.wysaid.listeners.VideoResponseListener;
import org.wysaid.models.FilterItem;

import java.util.List;

/**
 * Created by niek on 10/07/2017.
 */

public class GPUVideoView extends FrameLayout implements View.OnClickListener {
    private static final int THUMBNAIL_GENERATION_DELAY = 50;
    private static final int HIDE_CONTROLLER_DELAY = 3000;
    private static final int DONT_HIDE_CONTROLLER = Integer.MAX_VALUE;

    private VideoPlayerGLTextureView mGLTextureView;
    private ImageView mIvOverlay;
    private View mBtnPauseOrPlay;

    private Handler mHandlerReset = new Handler();
    private Runnable mRunnableReset;
    private Animation mAnimationFadeIn, mAnimationFadeOut;
    private boolean mAnimationInProgress, mPaused, mShouldHideControls = true;

    private Context mContext;

    private List<FilterItem> mThumbnailFilterItems;

    private VideoControlListener mVideoControlListener;

    public GPUVideoView(@NonNull Context context) {
        this(context, null);
    }

    public GPUVideoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public GPUVideoView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        LayoutInflater.from(context).inflate(R.layout.view_gpu_video, this, true);

        mContext = context;

        mGLTextureView = findViewById(R.id.gpuVideoViewPlayer);
        mIvOverlay = findViewById(R.id.gpuImageViewOverlay);
        mBtnPauseOrPlay = findViewById(R.id.playOrPause);

        // pause button animations
        mAnimationFadeIn = new AlphaAnimation(0f, 1f);
        mAnimationFadeOut = new AlphaAnimation(1f, 0f);
        mAnimationFadeIn.setDuration(300);
        mAnimationFadeOut.setDuration(300);
        mAnimationFadeIn.setFillAfter(true);
        mAnimationFadeOut.setFillAfter(true);

        setAnimationListeners();

        mGLTextureView.setOnClickListener(this);
        mBtnPauseOrPlay.setOnClickListener(this);

        mBtnPauseOrPlay.setActivated(!mPaused);

        // the reset runnable that will hide this controller
        mRunnableReset = new Runnable() {
            @Override
            public void run() {
                hideViews();
            }
        };
    }

    @Override
    public void onClick(View v) {
        if ("playOrPause".equals(v.getTag()) && mBtnPauseOrPlay.getVisibility() == VISIBLE) {
            mPaused = !mPaused;
            if(mPaused) {
                mVideoControlListener.playerPause();
            } else {
                mVideoControlListener.playerResume();
            }
        } else if (mBtnPauseOrPlay.getVisibility() == INVISIBLE) {
            mBtnPauseOrPlay.startAnimation(mAnimationFadeIn);
        }

        if(mShouldHideControls) initResetHandler(HIDE_CONTROLLER_DELAY);
    }

    private void initResetHandler(int delayAmount) {
        mHandlerReset.removeCallbacks(mRunnableReset);
        mHandlerReset.postDelayed(mRunnableReset, delayAmount);
    }

    private void hideViews() {
        mBtnPauseOrPlay.startAnimation(mAnimationFadeOut);
    }

    public void stopHidingControls() {
        mShouldHideControls = false;

        mHandlerReset.removeCallbacks(mRunnableReset);
        if (mBtnPauseOrPlay.getVisibility() == INVISIBLE) {
            mBtnPauseOrPlay.startAnimation(mAnimationFadeIn);
        }
    }

    public void startHidingControls() {
        mShouldHideControls = true;

        initResetHandler(HIDE_CONTROLLER_DELAY);
    }

    public void setVideoAndFilters(final Uri videoUri, final List<FilterItem> filterItems,
                                   final Bitmap thumbnailBitmap, final List<FilterItem> thumbnailFilterItems,
                                   final VideoResponseListener videoResponseListener,
                                   final BitmapGeneratedListener bitmapGeneratedListener,
                                   boolean useExoPlayer) {
        mThumbnailFilterItems = thumbnailFilterItems;

        // TODO: this bastard right here causes the Video Editor Fragment to load up slowly
//        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
//        retriever.setDataSource(mContext, videoUri);
//        mIvOverlay.setImageBitmap(retriever.getFrameAtTime(0, OPTION_CLOSEST_SYNC));

        mGLTextureView.setVideoUri(videoUri, filterItems,
                thumbnailBitmap, thumbnailFilterItems,
                videoResponseListener,
                new FrameRendererCreatedListener() {
                    @Override
                    public void frameRendererIsCreated() {
                        mGLTextureView.setRenderSize(250, 250);

                        for (FilterItem filterItem : filterItems) {
                            mGLTextureView.addFilterWithConfig(filterItem.getFilter());
                        }
                    }

                    @Override
                    public void thumbnailImageHasBeenSet() {
                        processThumbnails(0, bitmapGeneratedListener);
                    }
                }, useExoPlayer);
    }

    private void processThumbnails(final int index, final BitmapGeneratedListener bitmapGeneratedListener) {
        if (mThumbnailFilterItems != null && mThumbnailFilterItems.size() > 0) {
            new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                @Override
                public void run() {
                    mGLTextureView.getThumbnailWithConfig(mThumbnailFilterItems.get(index).getFilter(), new VideoPlayerGLTextureView.QueryResultBitmapCallback() {
                        @Override
                        public void get(Bitmap bitmap) {
                            bitmapGeneratedListener.newThumbnailGenerated(index, mThumbnailFilterItems.get(index).getResource(), bitmap);
                            if (index < mThumbnailFilterItems.size() - 1)
                                processThumbnails(index + 1, bitmapGeneratedListener);
                        }
                    });
                }
            }, THUMBNAIL_GENERATION_DELAY);
        }
    }

    public void start() {
        mGLTextureView.getPlayer().setVolume(0f, 0f);
        mGLTextureView.getPlayer().start();
        setViewVisibility(mIvOverlay, INVISIBLE);
    }

    public MediaPlayer getPlayer() {
        return mGLTextureView.getPlayer();
    }

    public void setFilterValueAtIndex(final String value, final int index) {
        mGLTextureView.setFilterValueAtIndex(value, index);
    }

    public void rotate90Degrees() {
        mGLTextureView.rotate90Degrees();
    }

    public void handleCrop(final String cropInfo) {
        String[] floats = cropInfo.split("\\s+");
        float width = Float.parseFloat(floats[2]);
        float height = Float.parseFloat(floats[3]);

        mGLTextureView.handleCrop(width, height);
    }

    public void pause() {
        mPaused = true;
        mBtnPauseOrPlay.setActivated(false);
        mGLTextureView.pause();
    }

    public void resume() {
        mPaused = false;
        mBtnPauseOrPlay.setActivated(true);
        mGLTextureView.resume();
    }

    public void seekTo(long position) {
        mGLTextureView.seekTo(position);
    }

    public void release() {
        mGLTextureView.release();
    }

    private void setViewVisibility(final View view, final int visibility) {
        ((Activity) mContext).runOnUiThread(new Runnable() {
            public void run() {
                view.setVisibility(visibility);
            }
        });
    }

    public long getVideoDuration() {
        return mGLTextureView.getVideoDuration();
    }

    private void setAnimationListeners() {
        mAnimationFadeIn.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                mAnimationInProgress = true;

                mBtnPauseOrPlay.setVisibility(VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                mAnimationInProgress = false;
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
            }
        });

        mAnimationFadeOut.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                mAnimationInProgress = true;
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                mAnimationInProgress = false;

                mBtnPauseOrPlay.setVisibility(INVISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
            }
        });
    }

    public void setVideoControlListener(VideoControlListener videoControlListener) {
        mVideoControlListener = videoControlListener;
    }
}
