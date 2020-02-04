package org.wysaid.models;

import android.graphics.Bitmap;

/**
 * Created by niek on 29/06/2017.
 */

public class Thumbnail {
    private Bitmap mImage;
    private String mResource;

    public Thumbnail(String resource, Bitmap thumbnailImage) {
        mResource = resource;
        mImage = thumbnailImage;
    }

    public void setImage(Bitmap image) {
        mImage = image;
    }

    public String getResource() {
        return mResource;
    }

    public Bitmap getImage() {
        return mImage;
    }
}
