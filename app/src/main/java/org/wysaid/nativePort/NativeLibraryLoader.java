package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/7/30.
 */
public class NativeLibraryLoader {

    private static boolean mLibraryLoaded = false;

    public static void load() {
        if (mLibraryLoaded) return;

        mLibraryLoaded = true;

        System.loadLibrary("CGE");
        System.loadLibrary("CGEExt");
    }
}
