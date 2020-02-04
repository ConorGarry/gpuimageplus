package org.wysaid.animations;

import android.animation.Animator;
import android.animation.ObjectAnimator;
import android.view.View;

/**
 * Created by niek on 15/04/2016.
 */
public class AnimationSlideVertical {
    private View mView1, mView2;
    private int mDuration;
    private AnimationListener mOnAnimationCallback;

    /***
     * Instantiate a new AnimationSlideVertical object.
     *
     * @param view1         the view to slide out
     * @param view2         the view to slide in
     * @param slideDuration the duration in milliseconds for each slide to last
     */
    public AnimationSlideVertical(View view1, View view2, int slideDuration) {
        mView1 = view1;
        mView2 = view2;
        mDuration = slideDuration;
    }

    /***
     * Instantiate a new AnimationSlideVertical object with a callback.
     *
     * @param view1               the view to slide out
     * @param view2               the view to slide in
     * @param slideDuration       the duration in milliseconds for each slide to last
     * @param onAnimationCallback the callback that gets called before and after the animation process
     */
    public AnimationSlideVertical(View view1, View view2, int slideDuration, AnimationListener onAnimationCallback) {
        mView1 = view1;
        mView2 = view2;
        mDuration = slideDuration;
        mOnAnimationCallback = onAnimationCallback;
    }

    /***
     * Start the slide animation.
     */
    public void start(boolean slideUp) {
        final int layoutHeight = mView1.getHeight();

        ObjectAnimator objectAnimatorView1 = ObjectAnimator.ofFloat(mView1, "translationY", 0, slideUp ? -layoutHeight : layoutHeight);
        objectAnimatorView1.setDuration(mDuration * 2);
        objectAnimatorView1.start();
        objectAnimatorView1.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {

            }

            @Override
            public void onAnimationEnd(Animator animation) {
                mView1.setVisibility(View.GONE);
                mView1.setY(0);
            }

            @Override
            public void onAnimationCancel(Animator animation) {

            }

            @Override
            public void onAnimationRepeat(Animator animation) {

            }
        });

        ObjectAnimator objectAnimatorView2 = ObjectAnimator.ofFloat(mView2, "translationY", slideUp ? layoutHeight : -layoutHeight, 0);
        objectAnimatorView2.setDuration(mDuration * 2);
        objectAnimatorView2.start();

        mView1.setVisibility(View.VISIBLE);
        mView2.setVisibility(View.VISIBLE);
    }
}