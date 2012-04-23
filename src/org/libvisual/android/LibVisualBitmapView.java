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
        
    private boolean videoInitialized = false;
    private Bitmap curBitmap;
    private VisVideo curBVideo, curAVideo;
    

        
    /* implementend by liblvclient.so */
    private static native boolean fpsInit();
    private static native void renderVisual(Bitmap bitmap, int binPtr, int videoPtr);



        
    /** constructor */
    public LibVisualBitmapView(Context context) 
    {
        super(context);
            
        /* initialize actor + input */
        fpsInit();
    }

            
    /** This is called during layout when the size of this view has changed. 
        If you were just added to the view hierarchy, you're called with the 
        old values of 0. */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        /* free previous Bitmap */
        if(curBitmap != null)
        {
            curBitmap.recycle();
            curBitmap = null;
        }

        /* create bitmap */
        curBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        /* validate bitmap */
        if(curBitmap.getConfig() != Bitmap.Config.ARGB_8888)
        {
            Log.e(TAG, "Bitmap format is not RGBA_8888 !");
            return;
        }

        Log.i(TAG, "onSizeChanged(): "+w+"x"+h+" stride: "+curBitmap.getRowBytes()+" (prev: "+oldw+"x"+oldh+")");

            
        /* create new VisVideo object for this bitmap */
        curBVideo = new VisVideo();
        curBVideo.setAttributes(w, h, curBitmap.getRowBytes(), 
                             VisVideo.VISUAL_VIDEO_DEPTH_32BIT);

        initVideo(w,h);
        //~ if(!videoInitialized)
        //~ {
            //~ initVideo(w, h);
            //~ videoInitialized = true;
        //~ }
        //~ else
        //~ {     
            //~ /* get depth of current actor */
            //~ int depth;
            //~ int depthflag = LibVisual.curActor.getSupportedDepth();
            //~ if(depthflag == VisVideo.VISUAL_VIDEO_DEPTH_GL)
            //~ {
                //~ depth = VisVideo.depthGetHighest(depthflag);
            //~ }
            //~ else
            //~ {
                //~ depth = VisVideo.depthGetHighestNoGl(depthflag);
            //~ }
                
            //~ curAVideo.setAttributes(w, h,
                                 //~ w*VisVideo.bppFromDepth(depth), depth);
            //~ curBVideo.setAttributes(w, h, 
                                    //~ curBitmap.getRowBytes(),
                                    //~ VisVideo.VISUAL_VIDEO_DEPTH_32BIT);
                
            //~ /* create new VisVideo object for this bitmap */
            //~ LibVisual.curActor.videoNegotiate(VisVideo.VISUAL_VIDEO_DEPTH_32BIT, false, false);
        //~ }

        /* realize bin */
        LibVisual.curBin.realize();
        LibVisual.curBin.sync(false);
        LibVisual.curBin.depthChanged();
            
    }

       
    /** called whenever the contents of this view are drawn */
    @Override 
    protected void onDraw(Canvas canvas) 
    {
        if(curBitmap == null)
        {
            return;
        }
            
        /* render */
        renderVisual(curBitmap, LibVisual.curBin.VisBin, curBVideo.VisVideo);
        canvas.drawBitmap(curBitmap, 0, 0, null);
            
        /* force a redraw, with a different time-based pattern. */
        invalidate();
    }

    /** initialize VisVideo for actor + buffer bitmap */
    void initVideo(int width, int height)
    {

            
        /* get depth of current actor */
        int depth;
        int depthflag = LibVisual.curActor.getSupportedDepth();
        if(depthflag == VisVideo.VISUAL_VIDEO_DEPTH_GL)
        {
            depth = VisVideo.depthGetHighest(depthflag);
        }
        else
        {
            depth = VisVideo.depthGetHighestNoGl(depthflag);
        }

        /* create new VisVideo for actor */
        VisVideo curAVideo = new VisVideo();
            
        curAVideo.setAttributes(width, height,
                             width*VisVideo.bppFromDepth(depth),
                             depth);
        curAVideo.allocateBuffer();

        /* set depth of bin */
        LibVisual.curBin.setDepth(depth);

        /* set video for bin */
        LibVisual.curBin.setVideo(curAVideo);

        /* connect actor & input to bin */
        LibVisual.curBin.connect(LibVisual.curActor, LibVisual.curInput);

    }

}
