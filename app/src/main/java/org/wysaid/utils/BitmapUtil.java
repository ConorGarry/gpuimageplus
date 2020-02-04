package org.wysaid.utils;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Point;
import android.media.ThumbnailUtils;
import androidx.annotation.NonNull;
import android.util.Size;

/**
 * Created by niek on 21/07/2017.
 */

public class BitmapUtil {
    private static final int ROTATION_90_DEGREES = 90;
    private static final int THUMBNAIL_SIZE = 250;

    public static Bitmap generateThumbnailBitmap(@NonNull Bitmap sourceBitmap, int amountOf90DegreeRotations, Point position, Size size) {
        return generateBitmap(sourceBitmap, amountOf90DegreeRotations, position, size, true);
    }

    public static Bitmap generateFullBitmap(@NonNull Bitmap sourceBitmap, int amountOf90DegreeRotations, Point position, Size size) {
        return generateBitmap(sourceBitmap, amountOf90DegreeRotations, position, size, false);
    }

    private static Bitmap generateBitmap(@NonNull Bitmap sourceBitmap, int amountOf90DegreeRotations, Point position, Size size, boolean isThumbnail) {
        Matrix matrix = new Matrix();
        matrix.postRotate(ROTATION_90_DEGREES * amountOf90DegreeRotations);
        Bitmap adjustedBitmap = Bitmap.createBitmap(sourceBitmap,
                position != null ? position.x : 0, position != null ? position.y : 0,
                size != null ? size.getWidth() : sourceBitmap.getWidth(), size != null ? size.getHeight() : sourceBitmap.getHeight(),
                matrix, true);
        return isThumbnail ? ThumbnailUtils.extractThumbnail(adjustedBitmap, THUMBNAIL_SIZE, THUMBNAIL_SIZE) : adjustedBitmap;
    }

    public static int getThumbnailSize() {
        return THUMBNAIL_SIZE;
    }
}
