package org.wysaid.listeners;

import android.media.MediaPlayer;

import com.google.android.exoplayer2.ExoPlayer;

/**
 * Created by niek on 27/06/2017.
 */

public interface VideoResponseListener {
    void playerIsPrepared(MediaPlayer mediaPlayer, ExoPlayer exoPlayer, int videoDuration);

    void playerIsCompleted(boolean replay);

    boolean playerIsFailed(MediaPlayer mp, int what, int extra);
}