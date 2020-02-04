package org.wysaid.utils;

/**
 * Created by niek on 28/11/2017.
 */

public enum FocusType {
    NO_FOCUS(0), RADIAL(1), LINEAR(2);
    private final int value;

    FocusType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}