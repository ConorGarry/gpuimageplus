package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.util.Log;

import org.wysaid.common.Common;

import java.util.Locale;

import static java.lang.String.format;

/**
 * Created by wysaid on 15/7/8.
 * Edited by niek
 */

public class CGENativeLibrary {

    static {
        NativeLibraryLoader.load();
    }

    public interface LoadImageCallback {
        Bitmap loadImage(String name, Object arg);

        void loadImageOK(Bitmap bmp, Object arg);
    }

    private static LoadImageCallback loadImageCallback;
    private static Object callbackArg;

    public static void setLoadImageCallback(LoadImageCallback callback, Object arg) {
        loadImageCallback = callback;
        callbackArg = arg;
    }

    public static class TextureResult {
        int texID;
        int width, height;

        TextureResult() {
            // empty constructor for NDK
        }

        TextureResult(int newTexID, int newWidth, int netHeight) {
            texID = newTexID;
            width = newWidth;
            height = netHeight;
        }

        @Override
        public String toString() {
            return format(Locale.US, "texture id: %d, width: %d, height: %d", texID, width, height);
        }

        public String toFilterConfig() {
            return format(Locale.US, "%d %d %d", texID, width, height);
        }
    }

    //will be called from jni.
    public static TextureResult loadTextureByName(String sourceName) {
        if (loadImageCallback == null) {
            Log.i(Common.LOG_TAG, "The loading callback is not set!");
            return null;
        }

        Bitmap bmp = loadImageCallback.loadImage(sourceName, callbackArg);

        if (bmp == null) {
            return null;
        }

        TextureResult result = loadTextureByBitmap(bmp);

        loadImageCallback.loadImageOK(bmp, callbackArg);
        return result;
    }

    // may be called from jni.
    public static TextureResult loadTextureByBitmap(Bitmap bmp) {
        if (bmp == null) {
            return null;
        }

        return new TextureResult(Common.genNormalTextureID(bmp), bmp.getWidth(), bmp.getHeight());
    }
}
