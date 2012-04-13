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

        
    private Bitmap mBitmap;
    private VisActor curActor;
    private VisInput curInput;
    private VisMorph curMorph;
    private VisBin curBin;
    private VisVideo curBVideo;
        
    /* implementend by liblvclient.so */
    private static native boolean init();
    private static native boolean deinit();
    private static native void renderVisual(Bitmap bitmap, int binPtr, int videoPtr);



        
    /** constructor */
    public LibVisualBitmapView(Context context, 
                               VisActor a,
                               VisInput i,
                               VisMorph m,
                               VisBin b) 
    {
        super(context);


        /* save libvisual objects */
        curActor = a;
        curInput = i;
        curMorph = m;
        curBin = b;
    }

            
    /** This is called when the view is attached to a window. */
    @Override
    protected void onAttachedToWindow()
    {
        super.onAttachedToWindow();
        Log.i(TAG,"onAttachedToWindow()");

            
        /* get depth of current actor */
        int depth;
        int depthflag = curActor.getSupportedDepth();
        if(depthflag == VisVideo.VISUAL_VIDEO_DEPTH_GL)
        {
            depth = VisVideo.depthGetHighest(depthflag);
        }
        else
        {
            depth = VisVideo.depthGetHighestNoGl(depthflag);
        }

        /* set depth of bin */
        curBin.setDepth(depth);

        /* connect actor & input to bin */
        curBin.connect(curActor.VisActor, curInput.VisInput);
            
        /* initialize actor + input */
        init();
    }


    /** This is called during layout when the size of this view has changed. 
        If you were just added to the view hierarchy, you're called with the 
        old values of 0. */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        /* free previous Bitmap */
        if(mBitmap != null)
        {
            mBitmap.recycle();
            mBitmap = null;
        }

        /* create bitmap */
        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        /* validate bitmap */
        if(mBitmap.getConfig() != Bitmap.Config.ARGB_8888)
        {
            Log.e(TAG, "Bitmap format is not RGBA_8888 !");
            return;
        }

        Log.i(TAG, "onSizeChanged(): "+w+"x"+h+" stride: "+mBitmap.getRowBytes()+" (prev: "+oldw+"x"+oldh+")");
            
        /* create new VisVideo object for this bitmap */
        curBVideo = new VisVideo();
        curBVideo.setAttributes(w, h, 
                            mBitmap.getRowBytes(), 
                            VisVideo.VISUAL_VIDEO_DEPTH_32BIT);
            
        /* get width from actor */
        int actorDepth = curActor.getSupportedDepth();
        int videoDepth;
        if(actorDepth == VisVideo.VISUAL_VIDEO_DEPTH_GL)
        {
            videoDepth = VisVideo.depthGetHighest(actorDepth);
        }
        else
        {
            videoDepth = VisVideo.depthGetHighestNoGl(actorDepth);
        }

        /* create new VisVideo for actor */
        VisVideo avideo = new VisVideo();
        avideo.setAttributes(w, h,
                             w*VisVideo.bppFromDepth(videoDepth),
                             videoDepth);
        avideo.allocateBuffer();

        /* set video for bin */
        curBin.setVideo(avideo.VisVideo);

        /* realize bin */
        curBin.realize();
        curBin.sync(false);
        curBin.depthChanged();
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
        renderVisual(mBitmap, curBin.VisBin, curBVideo.VisVideo);
        canvas.drawBitmap(mBitmap, 0, 0, null);
            
        /* force a redraw, with a different time-based pattern. */
        invalidate();
    }

}
