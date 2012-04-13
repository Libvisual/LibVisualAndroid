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
        
    private LibVisualSettings s;
    private Bitmap mBitmap;


    /* implementend by liblvclient.so */
    private static native boolean init();
    private static native boolean deinit();
    private static native boolean setBitmap(Bitmap bitmap);
    private static native boolean setActor(String name);
    private static native void getActor();
    private static native boolean setMorph(String name);
    private static native void getMorph();
    private static native boolean setInput(String name);
    private static native void getInput();
    private static native void renderVisual(Bitmap bitmap);


    /** constructor */
    public LibVisualBitmapView(Context context) 
    {
        super(context);

        /* get preferences */
        s = new LibVisualSettings(context);

        /* get defaults */
        String default_do_morph = context.getString(R.string.default_do_morph);
        String default_prevent_dimming = context.getString(R.string.default_prevent_dimming);
        String default_morph = context.getString(R.string.default_plugin_morph);
        String default_input = context.getString(R.string.default_plugin_input);
        String default_actor = context.getString(R.string.default_plugin_actor);
            
        /* read prefs values */
        Boolean doMorph = s.getBoolean("prefs_do_morph", (default_do_morph == "true" ? 
                                                          true : 
                                                          false));
        Boolean preventDimming = s.getBoolean("prefs_prevent_dimming",
                                                (default_prevent_dimming == "true" ? 
                                                          true : 
                                                          false));
        String input = s.getString("prefs_input", default_input);
        String actor = s.getString("prefs_actor", default_actor);
        String morph = s.getString("prefs_morph", default_morph);

        Log.v(TAG, "Input: \""+input+"\" Morph: \""+morph+"\" Actor: \""+actor);

            
        /* set initial plugins from preferences */
        /** @todo error handling */
        if(!setInput(input))
            Log.e(TAG, "Failed to set input");
            
        if(!setMorph(morph))
            Log.e(TAG, "Failed to set morph");
            
        if(!setActor(actor))
            Log.e(TAG, "Failed to set actor");

        /* don't dim screen */
        if(preventDimming)
            setKeepScreenOn(true);
        
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
