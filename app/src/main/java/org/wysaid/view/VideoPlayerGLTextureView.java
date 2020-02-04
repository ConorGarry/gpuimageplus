package org.wysaid.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.net.Uri;
import android.opengl.GLES20;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.DefaultLoadControl;
import com.google.android.exoplayer2.DefaultRenderersFactory;
import com.google.android.exoplayer2.ExoPlaybackException;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.ExoPlayerFactory;
import com.google.android.exoplayer2.PlaybackParameters;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.Timeline;
import com.google.android.exoplayer2.extractor.mp4.Mp4Extractor;
import com.google.android.exoplayer2.source.ExtractorMediaSource;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.TrackGroupArray;
import com.google.android.exoplayer2.trackselection.DefaultTrackSelector;
import com.google.android.exoplayer2.trackselection.TrackSelectionArray;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DataSpec;
import com.google.android.exoplayer2.upstream.DefaultAllocator;
import com.google.android.exoplayer2.upstream.FileDataSource;

import org.wysaid.common.Common;
import org.wysaid.listeners.FrameRendererCreatedListener;
import org.wysaid.listeners.VideoResponseListener;
import org.wysaid.models.FilterItem;
import org.wysaid.nativePort.CGEFrameRenderer;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.utils.FilterAdjustHelper;
import org.wysaid.utils.TextureRenderer;

import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.google.android.exoplayer2.DefaultLoadControl.DEFAULT_BUFFER_FOR_PLAYBACK_AFTER_REBUFFER_MS;
import static com.google.android.exoplayer2.DefaultLoadControl.DEFAULT_BUFFER_FOR_PLAYBACK_MS;
import static com.google.android.exoplayer2.DefaultLoadControl.DEFAULT_MAX_BUFFER_MS;
import static com.google.android.exoplayer2.DefaultLoadControl.DEFAULT_MIN_BUFFER_MS;

/**
 * Created by Niek on 19/06/19.
 */

public class VideoPlayerGLTextureView extends GLTextureView implements GLTextureView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final String LOG_TAG = Common.LOG_TAG;

    private SurfaceTexture mSurfaceTexture;
    private int mVideoTextureID;
    private CGEFrameRenderer mFrameRenderer;

    private TextureRenderer.Viewport mRenderViewport = new TextureRenderer.Viewport();
    private float[] mTransformMatrix = new float[16];
    private boolean mIsUsingMask = false;

    public boolean isUsingMask() {
        return mIsUsingMask;
    }

    private float mMaskAspectRatio = 1.0f;

    private int mViewWidth = 1000;
    private int mViewHeight = 1000;

    public int getViewWidth() {
        return mViewWidth;
    }

    public int getViewheight() {
        return mViewHeight;
    }

    private int mVideoWidth, mVideoHeight;
    private int mVideoWidthCropped, mVideoHeightCropped;

    private boolean mFitFullView = false;

    protected CGEImageHandler mImageHandlerThumbnails;
    private MediaPlayer mPlayer;
    private SimpleExoPlayer mExoPlayer;

    private Uri mVideoUri;
    private boolean mUseExoPlayer;

    public interface OnSurfaceCreatedCallback {
        void surfaceCreated();
    }

    protected OnSurfaceCreatedCallback mSurfaceCreatedCallback;

    public interface PlayerInitializeCallback {

        //对player 进行初始化设置， 设置未默认启动的listener， 比如 bufferupdateListener.
        void initPlayer(MediaPlayer player);

        void initExoPlayer(SimpleExoPlayer player);
    }

    PlayerInitializeCallback mPlayerInitCallback;

    FrameRendererCreatedListener mFrameRendererCreatedListener;

    private List<FilterItem> mFilterItems;
    private List<FilterItem> mThumbnailFilterItems;

    VideoResponseListener mVideoResponseListener;
    private Bitmap mThumbnailBitmap;

    public synchronized void setVideoUri(final Uri uri, final List<FilterItem> filterItems,
                                         final Bitmap thumbnailBitmap, final List<FilterItem> thumbnailFilterItems,
                                         final VideoResponseListener videoResponseListener,
                                         final FrameRendererCreatedListener frameRendererCreatedListener,
                                         boolean useExoPlayer) {
        mVideoUri = uri;
        mVideoResponseListener = videoResponseListener;
        mFrameRendererCreatedListener = frameRendererCreatedListener;
        mFilterItems = filterItems;
        mThumbnailFilterItems = thumbnailFilterItems;
        mThumbnailBitmap = thumbnailBitmap;
        mUseExoPlayer = useExoPlayer;
    }

    public void rotate90Degrees() {
        calcViewport();
    }

    public void getThumbnailWithConfig(final String config, final QueryResultBitmapCallback callback) {
        if (mImageHandlerThumbnails == null)
            return;

        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mImageHandlerThumbnails == null) {
                    Log.e(LOG_TAG, "set config after release!!");

                    return;
                }

                Bitmap bitmap = mImageHandlerThumbnails.getThumbnailWithConfig(config);
                callback.get(bitmap);
            }
        });
    }

    public interface QueryResultBitmapCallback {
        void get(Bitmap bitmap);
    }

    public synchronized void addFilterWithConfig(final String config) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mFrameRenderer != null) {
                    mFrameRenderer.addFilterWithConfig(config);
                } else {
                    Log.e(LOG_TAG, "setFilterWithConfig after release!!");
                }
            }
        });
    }

    public void setFilterValueAtIndex(final String value, final int index) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mFrameRenderer != null) {
                    mFrameRenderer.setFilterValueAtIndex(value, index);
                } else {
                    Log.e(LOG_TAG, "setFilterValue after release!!");
                }
            }
        });
    }

    public void setRenderSize(final int renderWidth, final int renderHeight) {
        if (mFrameRenderer == null)
            return;

        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mFrameRenderer == null) {
                    Log.e(LOG_TAG, "set render size after release!!");

                    return;
                }
                mFrameRenderer.setRenderSize(renderWidth, renderHeight);
            }
        });
    }

    public interface SetMaskBitmapCallback {
        void setMaskOK(CGEFrameRenderer recorder);
    }

    public void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle) {
        setMaskBitmap(bmp, shouldRecycle, null);
    }

    //注意， 当传入的bmp为null时， SetMaskBitmapCallback 不会执行.
    public void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle, final SetMaskBitmapCallback callback) {

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mFrameRenderer == null) {
                    Log.e(LOG_TAG, "setMaskBitmap after release!!");
                    return;
                }

                if (bmp == null) {
                    mFrameRenderer.setMaskTexture(0, 1.0f);
                    mIsUsingMask = false;
                    calcViewport();
                    return;
                }

                int texID = Common.genNormalTextureID(bmp, GLES20.GL_NEAREST, GLES20.GL_CLAMP_TO_EDGE);

                mFrameRenderer.setMaskTexture(texID, bmp.getWidth() / (float) bmp.getHeight());
                mIsUsingMask = true;
                mMaskAspectRatio = bmp.getWidth() / (float) bmp.getHeight();

                if (callback != null) {
                    callback.setMaskOK(mFrameRenderer);
                }

                if (shouldRecycle)
                    bmp.recycle();

                calcViewport();
            }
        });
    }

    public synchronized MediaPlayer getPlayer() {
        if (mPlayer == null) {
            Log.e(LOG_TAG, "Player is not initialized!");
        }
        return mPlayer;
    }

    public synchronized ExoPlayer getExoPlayer() {
        if (mExoPlayer == null) {
            Log.e(LOG_TAG, "ExoPlayer is not initialized!");
        }

        return mExoPlayer;
    }

    public interface OnCreateCallback {
        void createOK();
    }

    private OnCreateCallback mOnCreateCallback;

    public VideoPlayerGLTextureView(@NonNull Context context) {
        this(context, null);
    }

    public VideoPlayerGLTextureView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        setOpaque(false);

        Log.i(LOG_TAG, "MyGLTextureView Construct...");

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        setRenderer(this);
        setRenderMode(RENDERMODE_CONTINUOUSLY); // TODO: RENDERMODE_WHEN_DIRTY sometimes doesn't show the ExoPlayer, this, however, takes up more power

        mImageHandlerThumbnails = new CGEImageHandler();
        // mImageHandlerThumbs.setDrawerFlipScale(1.0f, -1.0f);

        Log.i(LOG_TAG, "MyGLTextureView Construct OK...");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "video player onSurfaceCreated...");

        if (mSurfaceCreatedCallback != null) {
            mSurfaceCreatedCallback.surfaceCreated();
        }

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);

        if (mOnCreateCallback != null) {
            mOnCreateCallback.createOK();
        }

        if (mVideoUri != null && (mSurfaceTexture == null || mVideoTextureID == 0)) {
            mVideoTextureID = Common.genSurfaceTextureID();
            mSurfaceTexture = new SurfaceTexture(mVideoTextureID);
            mSurfaceTexture.setOnFrameAvailableListener(VideoPlayerGLTextureView.this);

            if (mUseExoPlayer) {
                useExoUri();
            } else {
                useUri();
            }
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        mViewWidth = width;
        mViewHeight = height;

        calcViewport();
    }

    //must be in the OpenGL thread!
    public void release() {
        if (mPlayer != null || mExoPlayer != null) {
            queueEvent(new Runnable() {
                @Override
                public void run() {

                    Log.i(LOG_TAG, "Video player view release run...");

                    if (mPlayer != null) {
                        mPlayer.setSurface(null);
                        if (mPlayer.isPlaying())
                            mPlayer.stop();
                        mPlayer.release();
                        mPlayer = null;
                    }

                    if (mExoPlayer != null) {
                        mExoPlayer.setVideoSurface(null);
                        if (mExoPlayer.getPlayWhenReady()) {
                            mExoPlayer.stop();
                        }
                        mExoPlayer.release();
                        mExoPlayer = null;
                    }

                    if (mFrameRenderer != null) {
                        mFrameRenderer.release();
                        mFrameRenderer = null;
                    }

                    if (mSurfaceTexture != null) {
                        mSurfaceTexture.release();
                        mSurfaceTexture = null;
                    }

                    if (mImageHandlerThumbnails != null) {
                        mImageHandlerThumbnails.release();
                        mImageHandlerThumbnails = null;
                    }

                    if (mVideoTextureID != 0) {
                        GLES20.glDeleteTextures(1, new int[]{mVideoTextureID}, 0);
                        mVideoTextureID = 0;
                    }

                    mIsUsingMask = false;
                    mVideoResponseListener = null;

                    Log.i(LOG_TAG, "Video player view release OK");
                }
            });
        }
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "surfaceview onPause ...");

        super.onPause();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (mSurfaceTexture == null || mFrameRenderer == null) {
            return;
        }

        mSurfaceTexture.updateTexImage();

        if (mPlayer != null) {
            if (!mPlayer.isPlaying()) {
                return;
            }
        } else if (mExoPlayer == null) {
            return;
        }

        mSurfaceTexture.getTransformMatrix(mTransformMatrix);
        mFrameRenderer.update(mVideoTextureID, mTransformMatrix);

        mFrameRenderer.runProc();

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        GLES20.glEnable(GLES20.GL_BLEND);
        mFrameRenderer.render(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height);
        GLES20.glDisable(GLES20.GL_BLEND);
    }

    private long mTimeCount2 = 0;
    private long mFramesCount2 = 0;
    private long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        requestRender();

        if (mLastTimestamp2 == 0)
            mLastTimestamp2 = System.currentTimeMillis();

        long currentTimestamp = System.currentTimeMillis();

        ++mFramesCount2;
        mTimeCount2 += currentTimestamp - mLastTimestamp2;
        mLastTimestamp2 = currentTimestamp;
        if (mTimeCount2 >= 1e3) {
            mTimeCount2 -= 1e3;
            mFramesCount2 = 0;
        }
    }

    public void handleCrop(float width, float height) {
        mVideoWidthCropped = (int) width;
        mVideoHeightCropped = (int) height;

        calcViewport();
    }

    @SuppressWarnings("SuspiciousNameCombination")
    protected void calcViewport() {
        int width = mVideoWidthCropped > 0 ? mVideoWidthCropped : mVideoWidth;
        int height = mVideoHeightCropped > 0 ? mVideoHeightCropped : mVideoHeight;

        if (FilterAdjustHelper.sAmountOf90DegreeRotations % 2 == 0) {
            calcViewport(width, height);
        } else {
            calcViewport(height, width);
        }
    }

    protected void calcViewport(int videoWidth, int videoHeight) {
        float scaling;

        if (mIsUsingMask) {
            scaling = mMaskAspectRatio;
        } else {
            scaling = videoWidth / (float) videoHeight;
        }

        float viewRatio = mViewWidth / (float) mViewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if (mFitFullView) {
            //撑满全部view(内容大于view)
            if (s > 1.0) {
                w = (int) (mViewHeight * scaling);
                h = mViewHeight;
            } else {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            }
        } else {
            //显示全部内容(内容小于view)
            if (s > 1.0) {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            } else {
                h = mViewHeight;
                w = (int) (mViewHeight * scaling);
            }
        }

        mRenderViewport.width = w;
        mRenderViewport.height = h;
        mRenderViewport.x = (mViewWidth - mRenderViewport.width) / 2;
        mRenderViewport.y = (mViewHeight - mRenderViewport.height) / 2;
        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height));
    }

    private void useUri() {
        if (mPlayer != null) {
            mPlayer.stop();
            mPlayer.reset();
        } else {
            mPlayer = new MediaPlayer();
        }

        try {
            mPlayer.setDataSource(getContext(), mVideoUri);
            mPlayer.setSurface(new Surface(mSurfaceTexture));
        } catch (Exception e) {
            e.printStackTrace();
            Log.e(LOG_TAG, "useUri failed");

            return;
        }

        if (mPlayerInitCallback != null) {
            mPlayerInitCallback.initPlayer(mPlayer);
        }

        mPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mPlayer = mp;
                mVideoWidth = mPlayer.getVideoWidth();
                mVideoHeight = mPlayer.getVideoHeight();

                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if (mFrameRenderer == null) {
                            mFrameRenderer = new CGEFrameRenderer();

                            queueEvent(new Runnable() {
                                @Override
                                public void run() {
                                    Log.i(LOG_TAG, "setVideoUri...");

                                    if (mSurfaceTexture == null || mVideoTextureID == 0) {
                                        mVideoTextureID = Common.genSurfaceTextureID();
                                        mSurfaceTexture = new SurfaceTexture(mVideoTextureID);
                                        mSurfaceTexture.setOnFrameAvailableListener(VideoPlayerGLTextureView.this);
                                    }

                                    mFrameRendererCreatedListener.frameRendererIsCreated();

                                    if (mThumbnailBitmap != null && mImageHandlerThumbnails.initWithBitmap(mThumbnailBitmap)) {
                                        mFrameRendererCreatedListener.thumbnailImageHasBeenSet();
                                    } else {
                                        Log.e(LOG_TAG, "setImageBitmap: 初始化 handler 失败!");
                                    }

                                    useUri();
                                }
                            });
                        }

                        if (mFrameRenderer.init(mVideoWidth, mVideoHeight, mVideoWidth, mVideoHeight)) {
                            //Keep right orientation for source texture blending
                            mFrameRenderer.setSrcFlipScale(1.0f, -1.0f);
                            mFrameRenderer.setRenderFlipScale(1.0f, -1.0f);
                        } else {
                            Log.e(LOG_TAG, "Frame Recorder init failed!");
                        }

                        calcViewport();
                    }
                });

                if (mVideoResponseListener != null) {
                    mVideoResponseListener.playerIsPrepared(mp, null, mPlayer.getDuration());
                } else {
                    mPlayer.start();
                }

                Log.i(LOG_TAG, String.format("Video resolution 1: %d x %d", mVideoWidth, mVideoHeight));
            }
        });

        mPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(MediaPlayer mp, int what, int extra) {
                return mVideoResponseListener != null && mVideoResponseListener.playerIsFailed(mp, what, extra);
            }
        });

        try {
            mPlayer.prepareAsync();
        } catch (Exception e) {
            Log.e(LOG_TAG, String.format("Error handled: %s, play failure handler would be called!", e.toString()));
        }
    }

    private boolean mExoPlayerIsPrepared, mExoPlayerIsLoading;

    private void useExoUri() {
        mExoPlayer = ExoPlayerFactory.newSimpleInstance(
                getContext(),
                new DefaultRenderersFactory(getContext()),
                new DefaultTrackSelector(),
                new DefaultLoadControl.Builder()
                        .setAllocator(new DefaultAllocator(true, C.DEFAULT_BUFFER_SEGMENT_SIZE))
                        .setBufferDurationsMs(
                                DEFAULT_MIN_BUFFER_MS / 4,
                                DEFAULT_MAX_BUFFER_MS / 4,
                                DEFAULT_BUFFER_FOR_PLAYBACK_MS / 4,
                                DEFAULT_BUFFER_FOR_PLAYBACK_AFTER_REBUFFER_MS / 4)
                        .setTargetBufferBytes(C.LENGTH_UNSET)
                        .setPrioritizeTimeOverSizeThresholds(false)
                        .createDefaultLoadControl());

        try {
            mExoPlayer.setVideoSurface(new Surface(mSurfaceTexture));
        } catch (Exception e) {
            e.printStackTrace();
            Log.e(LOG_TAG, "useExoUri failed");

            return;
        }

        if (mPlayerInitCallback != null) {
            mPlayerInitCallback.initExoPlayer(mExoPlayer);
        }

        mExoPlayer.setVideoListener(new SimpleExoPlayer.VideoListener() {
            @Override
            public void onVideoSizeChanged(int width, int height, int unappliedRotationDegrees, float pixelWidthHeightRatio) {
                mVideoWidth = width;
                mVideoHeight = height;
            }

            @Override
            public void onRenderedFirstFrame() {

            }
        });

        mExoPlayer.addListener(new Player.EventListener() {
            @Override
            public void onLoadingChanged(boolean isLoading) {
                mExoPlayerIsLoading = isLoading;
            }

            @Override
            public void onPlayerStateChanged(boolean playWhenReady, int playbackState) {
                if (!mExoPlayerIsPrepared && playbackState == Player.STATE_READY) {
                    mExoPlayerIsPrepared = true;

                    TrackGroupArray trackGroupArray = mExoPlayer.getCurrentTrackGroups();
                    for (int i = 0; i < trackGroupArray.length; i++) {
                        for (int ii = 0; ii < trackGroupArray.get(i).length; ii++) {
                            Log.e(LOG_TAG, "tracker is: " + trackGroupArray.get(i).getFormat(ii).toString());
                        }
                    }

                    queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            if (mFrameRenderer == null) {
                                mFrameRenderer = new CGEFrameRenderer();

                                queueEvent(new Runnable() {
                                    @Override
                                    public void run() {
                                        Log.i(LOG_TAG, "setVideoUri...");

                                        if (mSurfaceTexture == null || mVideoTextureID == 0) {
                                            mVideoTextureID = Common.genSurfaceTextureID();
                                            mSurfaceTexture = new SurfaceTexture(mVideoTextureID);
                                            mSurfaceTexture.setOnFrameAvailableListener(VideoPlayerGLTextureView.this);
                                        }

                                        mFrameRendererCreatedListener.frameRendererIsCreated();

                                        if (mThumbnailBitmap != null && mImageHandlerThumbnails.initWithBitmap(mThumbnailBitmap)) {
                                            mFrameRendererCreatedListener.thumbnailImageHasBeenSet();
                                        } else {
                                            Log.e(LOG_TAG, "setImageBitmap: 初始化 handler 失败!");
                                        }
                                    }
                                });
                            }

                            if (mFrameRenderer.init(mVideoWidth, mVideoHeight, mVideoWidth, mVideoHeight)) {
                                //Keep right orientation for source texture blending
                                mFrameRenderer.setSrcFlipScale(1.0f, -1.0f);
                                mFrameRenderer.setRenderFlipScale(1.0f, -1.0f);
                            } else {
                                Log.e(LOG_TAG, "Frame Recorder init failed!");
                            }

                            calcViewport();
                        }
                    });

                    if (mVideoResponseListener != null) {
                        mVideoResponseListener.playerIsPrepared(null, mExoPlayer, (int) mExoPlayer.getDuration());
                        mExoPlayer.setPlayWhenReady(true);
                    } else {

                    }

                    Log.e(LOG_TAG, String.format("Video resolution 1: %d x %d", mVideoWidth, mVideoHeight));
                }
            }

            @Override
            public void onRepeatModeChanged(int repeatMode) {

            }

            @Override
            public void onShuffleModeEnabledChanged(boolean shuffleModeEnabled) {

            }

            @Override
            public void onPlayerError(ExoPlaybackException error) {

            }

            @Override
            public void onPositionDiscontinuity(int reason) {

            }

            @Override
            public void onPlaybackParametersChanged(PlaybackParameters playbackParameters) {

            }

            @Override
            public void onSeekProcessed() {

            }

            @Override
            public void onTimelineChanged(Timeline timeline, Object manifest, int reason) {

            }

            @Override
            public void onTracksChanged(TrackGroupArray trackGroups, TrackSelectionArray trackSelections) {

            }
        });

        DataSpec dataSpec = new DataSpec(mVideoUri);
        final FileDataSource fileDataSource = new FileDataSource();
        try {
            fileDataSource.open(dataSpec);
        } catch (FileDataSource.FileDataSourceException e) {
            e.printStackTrace();
        }

        DataSource.Factory factory = new DataSource.Factory() {
            @Override
            public DataSource createDataSource() {
                return fileDataSource;
            }
        };
        MediaSource videoSource = new ExtractorMediaSource(
                fileDataSource.getUri(),
                factory,
                Mp4Extractor.FACTORY,
                null,
                null
        );

        mExoPlayer.setPlayWhenReady(true);
        mExoPlayer.prepare(videoSource);
    }

    public void seekTo(long position) {
        if (mPlayer != null && Math.abs(mPlayer.getCurrentPosition() - position) > 16)
            mPlayer.seekTo((int) position);
        if (mExoPlayer != null && !mExoPlayerIsLoading)
            mExoPlayer.seekTo(position);
    }

    public void pause() {
        if (mPlayer != null) mPlayer.pause();
        if (mExoPlayer != null) mExoPlayer.setPlayWhenReady(false);
    }

    public void resume() {
        if (mPlayer != null) mPlayer.start();
        if (mExoPlayer != null) mExoPlayer.setPlayWhenReady(true);
    }

    public long getVideoDuration() {
        return (long) mPlayer.getDuration();
    }
}
