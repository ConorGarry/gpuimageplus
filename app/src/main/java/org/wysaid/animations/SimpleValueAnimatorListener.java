package org.wysaid.animations;

public interface SimpleValueAnimatorListener {
    void onAnimationStarted();
    void onAnimationUpdated(float scale);
    void onAnimationFinished();
}
