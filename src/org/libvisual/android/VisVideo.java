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

import android.util.Log;
import java.lang.Exception;



/** VisVideo wrapper */
public class VisVideo
{
    private final static String TAG = "VisVideo";
    public final static int VISUAL_VIDEO_DEPTH_NONE	   = 0;	/**< No video surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_8BIT	   = 1;	/**< 8 bits indexed surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_16BIT   = 2;	/**< 16 bits 5-6-5 surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_24BIT   = 4;	/**< 24 bits surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_32BIT   = 8;	/**< 32 bits surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_GL	   = 16;	/**< openGL surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_ENDLIST = 32;	/**< Used to mark the end of the depth list. */
    public final static int VISUAL_VIDEO_DEPTH_ERROR   = -1;	/**< Used when there is an error. */
    public final static int VISUAL_VIDEO_DEPTH_ALL	   = VISUAL_VIDEO_DEPTH_8BIT
                                | VISUAL_VIDEO_DEPTH_16BIT
                                | VISUAL_VIDEO_DEPTH_24BIT
                                | VISUAL_VIDEO_DEPTH_32BIT
                                | VISUAL_VIDEO_DEPTH_GL; /**< All graphical depths. */
        

    /** implemented by visual.c */
    private native int videoNew();
    private native void videoUnref(int videoPtr);
    private native void videoAllocateBuffer(int videoPtr);
    private native void videoSetAttributes(int videoPtr, int width, int height, int stride, int depth); 
    private static native int videoGetHighestDepth(int depth);
    private static native int videoGetHighestDepthNoGl(int depth);    
    private static native int videoBppFromDepth(int depth);
    
    /* current VisVideo object */
    public int VisVideo;    



        
    public VisVideo()
    {
        VisVideo = videoNew();
    }


    public void setAttributes(int width, int height, int stride, int depth)
    {
        videoSetAttributes(VisVideo, width, height, stride, depth);
    }

    public static int depthGetHighest(int depth)
    {
        return videoGetHighestDepth(depth);
    }

    public static int depthGetHighestNoGl(int depth)
    {
        return videoGetHighestDepthNoGl(depth);
    }

    public static int bppFromDepth(int depth)
    {
        return videoBppFromDepth(depth);
    }

    public void allocateBuffer()
    {
        videoAllocateBuffer(VisVideo);
    }
        
    @Override
    public void finalize()
    {
        videoUnref(VisVideo);
    }
}

