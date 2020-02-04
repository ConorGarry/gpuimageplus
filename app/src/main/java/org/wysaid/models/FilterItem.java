package org.wysaid.models;

import android.graphics.Bitmap;
import androidx.annotation.NonNull;

import org.wysaid.utils.FilterAdjustHelper;

/**
 * Created by niek on 27/06/2017.
 */

public class FilterItem {
    private static final String ADJUST = "adjust";
    private static final String FILTER_STRING = "@%1$s %2$s %3$s ";

    private String mFunction;
    private String mFilterName;
    private String mFilterResource;
    private double mDefaultAdjustValue, mMinAdjustValue, mMaxAdjustValue, mValueMultiplier;
    private double mProgress, mSavedProgress;

    private FilterItemUi mFilterItemUi;

    /**
     * Constructor mainly used for the common filters
     *
     * @param filter             name of the filter you want to apply
     * @param currentAdjustValue the default adjust value (of the seek bar)
     * @param minAdjustValue     the minimum adjust value (of the seek bar)
     * @param maxAdjustValue     the maximum adjust value (of the seek bar)
     */
    public FilterItem(@NonNull String title, @NonNull String filter, double currentAdjustValue, double minAdjustValue, double maxAdjustValue, double valueMultiplier) {
        mFunction = ADJUST;
        mFilterName = filter;
        mMinAdjustValue = minAdjustValue;
        mMaxAdjustValue = maxAdjustValue;
        mValueMultiplier = valueMultiplier;

        mDefaultAdjustValue = mSavedProgress = currentAdjustValue;

        mFilterItemUi = new FilterItemUi(title);
    }

    /**
     * Constructor mainly used for creating LUT filters
     *
     * @param filter         the name of the command
     * @param filterResource the name of the resource
     */
    public FilterItem(@NonNull String title, @NonNull String filter, @NonNull String filterResource) {
        mFunction = ADJUST;
        mFilterName = filter;
        mFilterResource = filterResource;
        mDefaultAdjustValue = mMinAdjustValue = mMaxAdjustValue = -1;

        mFilterItemUi = new FilterItemUi(title);
    }

    /**
     * Constructor
     *
     * @param filter         the name of the command
     */
    public FilterItem(@NonNull String title, @NonNull String filter) {
        mFunction = ADJUST;
        mFilterName = filter;
        mDefaultAdjustValue = mMinAdjustValue = mMaxAdjustValue = -1;

        mFilterItemUi = new FilterItemUi(title);
    }

    public void setProgress(double progress) {
        mProgress = progress;
    }

    public FilterItem setCurrentFilterResource(String filterResource) {
        mFilterResource = filterResource;
        return this;
    }

    public String getFilterName() {
        return mFilterName;
    }

    public String getFilter() {
        return String.format(FILTER_STRING, mFunction, mFilterName, getValue());
    }

    public String getValue() {
        String filterResource = mFilterResource != null ? mFilterResource : "";
        String filterValue = mMinAdjustValue == -1 ? "" : FilterAdjustHelper.getValue(mFilterName, mProgress);

        if(!"".equals(filterResource) && !"".equals(filterValue)) {
            return String.format("%1$s %2$s", filterValue, filterResource);
        } else if(!"".equals(filterResource)) {
            return String.format("%1$s", filterResource);
        } else if(!"".equals(filterValue)) {
            return String.format("%1$s", filterValue);
        } else {
            return "";
        }
    }

    public String getDefaultValue() {
        String filterResource = mFilterResource != null ? mFilterResource : "";
        String filterValue = mMinAdjustValue == -1 ? "" : FilterAdjustHelper.getValue(mFilterName, mDefaultAdjustValue);

        if(!"".equals(filterResource) && !"".equals(filterValue)) {
            return String.format("%1$s %2$s", filterValue, filterResource);
        } else if(!"".equals(filterResource)) {
            return String.format("%1$s", filterResource);
        } else if(!"".equals(filterValue)) {
            return String.format("%1$s", filterValue);
        } else {
            return "";
        }
    }

    public String getResource() {
        return mFilterResource;
    }

    public double getSavedProgress() {
        return mSavedProgress;
    }

    public double getMaxAdjustValue() {
        return mMaxAdjustValue;
    }

    public double getSeekBarMin() {
        return mMinAdjustValue;
    }

    public double getSeekBarMax() {
        return mMaxAdjustValue;
    }

    public double getSeekBarMultiplier() {
        return mValueMultiplier;
    }

    public double getDefaultAdjustValue() {
        return mDefaultAdjustValue;
    }

    public double saveProgress() {
        return mSavedProgress = mProgress;
    }

    public double cancelProgress() {
        return mProgress = mSavedProgress;
    }

    /////// UI

    public String getTitle() {
        return mFilterItemUi.getTitle();
    }

    public Bitmap getBitmapIcon() {
        return mFilterItemUi.getBitmapIcon();
    }

    public FilterItem setBitmapIcon(Bitmap bitmapIcon) {
        mFilterItemUi.setBitmapIcon(bitmapIcon);
        return this;
    }

    public String getNetProgress() {
        return String.valueOf(mProgress);
    }
}
