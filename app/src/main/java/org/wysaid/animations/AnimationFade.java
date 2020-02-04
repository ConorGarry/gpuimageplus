package org.wysaid.animations;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.view.View;

/**
 * Created by niek on 15/04/2016.
 */
public class AnimationFade {
    private View mView1, mView2;
    private int mDuration;
    private AnimationListener mOnAnimationCallback;
    private boolean mIsCrossFade;

    /***
     * Instantiate a new AnimationFade object.
     *
     * @param view1        the view to fade out
     * @param view2        the view to fade in
     * @param fadeDuration the duration in milliseconds for each fade to last
     */
    public AnimationFade(View view1, View view2, int fadeDuration) {
        mView1 = view1;
        mView2 = view2;
        mDuration = fadeDuration;
    }

    public AnimationFade(View view1, View view2, int fadeDuration, boolean crossfade) {
        mView1 = view1;
        mView2 = view2;
        mDuration = fadeDuration;
        mIsCrossFade = crossfade;
    }
    /***
     * Instantiate a new AnimationFade object with a callback.
     *
     * @param view1               the view to fade out
     * @param view2               the view to fade in
     * @param fadeDuration        the duration in milliseconds for each fade to last
     * @param onAnimationCallback the callback that gets called before and after the animation process
     */
    public AnimationFade(View view1, View view2, int fadeDuration, AnimationListener onAnimationCallback) {
        mView1 = view1;
        mView2 = view2;
        mDuration = fadeDuration;
        mOnAnimationCallback = onAnimationCallback;
    }

    public AnimationFade(View view1, View view2, int fadeDuration, boolean crossfade, AnimationListener onAnimationCallback) {
        mView1 = view1;
        mView2 = view2;
        mDuration = fadeDuration;
        mIsCrossFade = crossfade;
        mOnAnimationCallback = onAnimationCallback;
    }

    /***
     * Start the fade animation.
     */
    public void start() {
        if (mOnAnimationCallback != null) {
            mOnAnimationCallback.onAnimationStart();
        }

        mView1.setVisibility(View.VISIBLE);
        mView1.setAlpha(1f);

        mView2.setAlpha(0f);
        mView2.setVisibility(View.VISIBLE);
        if(mIsCrossFade){
            mView1.animate()
                    .alpha(0f)
                    .setDuration(mDuration)
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            mView1.clearAnimation();
                            mView1.setVisibility(View.GONE);
                            mView1.setAlpha(1f);
                            if (mOnAnimationCallback != null) {
                                mOnAnimationCallback.onAnimationComplete();
                            }
                        }
                    })
                    .start();
            mView2.animate()
                    .alpha(1f)
                    .setDuration(mDuration)
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            mView2.clearAnimation();
                        }
                    })
                    .start();
        }else {
            mView1.animate()
                    .alpha(0f)
                    .setDuration(mDuration)
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            mView1.setVisibility(View.GONE);
                            mView1.setAlpha(1f);
                            mView2.animate()
                                    .alpha(1f)
                                    .setDuration(mDuration)
                                    .setListener(new AnimatorListenerAdapter() {
                                        @Override
                                        public void onAnimationEnd(Animator animation) {
                                            if (mOnAnimationCallback != null) {
                                                mOnAnimationCallback.onAnimationComplete();
                                            }
                                        }
                                    });
                        }
                    });
        }
    }

    /***
     * Start the fade animation.
     */
    public void startSingle() {
        if (mOnAnimationCallback != null) {
            mOnAnimationCallback.onAnimationStart();
        }

        if (mView1 != null) {
            mView1.setVisibility(View.VISIBLE);
            mView1.animate()
                    .alpha(0f)
                    .setDuration(mDuration)
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            mView1.setVisibility(View.GONE);
                            mView1.setAlpha(1f);
                            if (mOnAnimationCallback != null) {
                                mOnAnimationCallback.onAnimationComplete();
                            }
                        }
                    });
        } else if (mView2 != null) {
            mView2.setAlpha(0f);
            mView2.setVisibility(View.VISIBLE);

            mView2.animate()
                    .alpha(1f)
                    .setDuration(mDuration)
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            if (mOnAnimationCallback != null) {
                                mOnAnimationCallback.onAnimationComplete();
                            }
                        }
                    });
        } else {
            return;
        }

    }
}