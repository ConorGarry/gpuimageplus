package org.wysaid.listeners;

/**
 * Created by niek on 27/06/2017.
 */

public interface VideoControlListener {

    void playerPause();

    void playerResume();

    void playerSeekTo(long position);
}