/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004-2006 Dennis Smit <ds@nerds-incorporated.org>
 * Copyright (C) 2012 Daniel Hiepler <daniel-lva@niftylight.de>         
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel-lva@niftylight.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

package org.libvisual.android;

import android.content.Context;
import android.view.View;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.Log;




class LibVisualBitmapView extends LibVisualView 
{
    private final static String TAG = "LibVisualBitmapView"; 
    private final int VISUAL_VIDEO_DEPTH_NONE	 = 0;	/**< No video surface flag. */
    private final int VISUAL_VIDEO_DEPTH_8BIT	 = 1;	/**< 8 bits indexed surface flag. */
    private final int VISUAL_VIDEO_DEPTH_16BIT	 = 2;	/**< 16 bits 5-6-5 surface flag. */
    private final int VISUAL_VIDEO_DEPTH_24BIT	 = 4;	/**< 24 bits surface flag. */
    private final int VISUAL_VIDEO_DEPTH_32BIT	 = 8;	/**< 32 bits surface flag. */
    private final int VISUAL_VIDEO_DEPTH_GL		 = 16;	/**< openGL surface flag. */
    private final int VISUAL_VIDEO_DEPTH_ENDLIST = 32;	/**< Used to mark the end of the depth list. */
    private final int VISUAL_VIDEO_DEPTH_ERROR	 = -1;	/**< Used when there is an error. */
    private final int VISUAL_VIDEO_DEPTH_ALL	 = VISUAL_VIDEO_DEPTH_8BIT
                                | VISUAL_VIDEO_DEPTH_16BIT
                                | VISUAL_VIDEO_DEPTH_24BIT
                                | VISUAL_VIDEO_DEPTH_32BIT
                                | VISUAL_VIDEO_DEPTH_GL; /**< All graphical depths. */

        
    private LibVisualSettings s;
    private Bitmap mBitmap;


    /* implementend by liblvclient.so */
    private static native boolean init();
    private static native boolean deinit();
    private static native boolean setBitmap(Bitmap bitmap);
    private static native boolean setActor(int actor);
    private static native boolean setMorph(int morph);
    private static native boolean setInput(int input);
    private static native boolean setBin(int bin);
    private static native void renderVisual(Bitmap bitmap);


    /** constructor */
    public LibVisualBitmapView(Context context, 
                               VisActor a,
                               VisInput i,
                               VisMorph m,
                               VisBin b) 
    {
        super(context);

        /* get preferences */
        s = new LibVisualSettings(context);

        
        /* prevent dimming of screen? */
        String default_prevent_dimming = context.getString(R.string.default_prevent_dimming);
        Boolean preventDimming = s.getBoolean("prefs_prevent_dimming",
                                                (default_prevent_dimming == "true" ? 
                                                          true : 
                                                          false));
        
        Log.v(TAG, "Prevent dimming: "+preventDimming);
                       
        /* don't dim screen */
        if(preventDimming)
            setKeepScreenOn(true);

        /* set actor */
        setActor(a.VisActor);

        /* set input */
        setInput(i.VisInput);

        /* set morph */
        setMorph(m.VisMorph);

        /* set bin */
        setBin(b.VisBin);

        /* set depth */
        //b.setSupportedDepth(VISUAL_VIDEO_DEPTH_ALL);
        //b.setPreferredDepth(VISUAL_VIDEO_DEPTH_32BIT);

        //int depthflag = a.getSupportedDepth();
        
    }


    /** This is called when the view is attached to a window. */
    @Override
    protected void onAttachedToWindow()
    {
        super.onAttachedToWindow();
        Log.i(TAG,"onAttachedToWindow()");

        /* initialize actor + input */
        init();
    }


    /** This is called during layout when the size of this view has changed. 
        If you were just added to the view hierarchy, you're called with the 
        old values of 0. */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        Log.i(TAG, "onSizeChanged(): "+w+"x"+h+" (prev: "+oldw+"x"+oldh+")");

            
        /* free previous Bitmap */
        if(mBitmap != null)
        {
            mBitmap.recycle();
            mBitmap = null;
        }

        /* create bitmap */
        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        /* initialize the libvisual view */
        /** @todo error handling */
        setBitmap(mBitmap);
    }

        
    /** This is called when the view is detached from a window. 
        At this point it no longer has a surface for drawing. */
    @Override
    protected void onDetachedFromWindow()
    {
        Log.i(TAG,"onDetachedToWindow()");    
            
        /** deinitialize libvisual view */
        deinit();
        super.onDetachedFromWindow();
    }  
        
       
    /** called whenever the contents of this view are drawn */
    @Override 
    protected void onDraw(Canvas canvas) 
    {
        if(mBitmap == null)
        {
            return;
        }
            
        /* render */
        //canvas.drawColor(0xFFCCCCCC);
        renderVisual(mBitmap);
        canvas.drawBitmap(mBitmap, 0, 0, null);
            
        /* force a redraw, with a different time-based pattern. */
        invalidate();
    }

}
