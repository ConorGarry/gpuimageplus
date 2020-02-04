package org.wysaid.utils;

import android.opengl.Matrix;
import androidx.annotation.NonNull;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;

/**
 * Created by niek on 20/06/2017.
 */

public class FilterAdjustHelper {
    public static int sAmountOf90DegreeRotations = 0;

    private static final String EXPOSURE = "exposure";
    private static final String TRANSFORMATION = "transformation";
    private static final String CONTRAST = "contrast";
    private static final String SATURATION = "saturation";
    private static final String SHADOWS = "shadows";
    private static final String HIGHLIGHTS = "highlights";
    private static final String WARMTH = "warmth";
    private static final String SHARPEN = "sharpen";
    private static final String VIGNETTE = "vignette";

    @NonNull
    public static String getValue(@NonNull String filter, double progress) {
        float value = (float) progress;

        switch (filter) {
            case EXPOSURE:
                return Double.toString(progress / 50.0f * 0.4f);

            case TRANSFORMATION:
                return getTransformationMatrix(progress, sAmountOf90DegreeRotations);

            case CONTRAST:
                if (value < 0.0f) {
                    value /= 200.0f;
                } else {
                    value /= 100.0f;
                }

                return Float.toString(value * 0.6f + 1.0f);

            case SATURATION:
                if (value < 0.0) {
                    value = value / 100.0f + 1.0f;
                } else {
                    value = value / 100.0f * 0.5f + 1.0f;
                }
                return Float.toString(value);

            case SHADOWS:
                return Double.toString(progress / 100.0f);

            case HIGHLIGHTS:
                return Double.toString(progress / 100.0f);

            case WARMTH:
                return Double.toString(progress / 100.0f * 0.8f);

            case SHARPEN:
                return Double.toString(progress / 100.0f * 0.7f);

            case VIGNETTE:
                return Double.toString(progress / 120.0f * 0.8f);

            default:
                return Double.toString(progress); // not done;
        }
    }

    @NonNull
    public static String getTransformationMatrix(double progress, int amountOf90DegreeRotations) {
        float rotateAmount = (float) (progress / (180f / 25f));
        float scaleAmount = (float) (1f + Math.abs(progress / 45.0f));

        float[] transformationMatrix = new float[16];
        Matrix.setRotateM(transformationMatrix, 0, 360 * (rotateAmount + 25 * amountOf90DegreeRotations) / 100, 0, 0, 1.0f);
        Matrix.scaleM(transformationMatrix, 0, scaleAmount, scaleAmount, 1f);

        return StringUtils.join(ArrayUtils.toObject(transformationMatrix), " ");
    }
}
