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




class LibVisualView extends View 
{
    private final static String TAG = "LibVisual/View";
        
    private LibVisualSettings s;
    private Bitmap mBitmap;


    /* implementend by liblvclient.so */
    private static native boolean init(Bitmap bitmap);
    private static native boolean deinit();
    private static native void renderVisual(Bitmap bitmap);
    private static native boolean setActor(String name);
    private static native void getActor();
    private static native boolean setMorph(String name);
    private static native void getMorph();
    private static native boolean setInput(String name);
    private static native void getInput();

        
    public LibVisualView(Context context) 
    {
        super(context);

        /* get preferences */
        s = new LibVisualSettings(context);

        /* get defaults */
        String default_do_morph = context.getString(R.string.default_do_morph);
        String default_morph = context.getString(R.string.default_plugin_morph);
        String default_input = context.getString(R.string.default_plugin_input);
        String default_actor = context.getString(R.string.default_plugin_actor);
            
        /* read prefs values */
        Boolean doMorph = s.getBoolean("prefs_do_morph", (default_do_morph == 
                                                          "true" ? 
                                                          true : 
                                                          false));
        String input = s.getString("prefs_input", default_input);
        String actor = s.getString("prefs_actor", default_actor);
        String morph = s.getString("prefs_morph", default_morph);

        Log.v(TAG, "Input: \""+input+"\" Morph: \""+morph+"\" Actor: \""+actor);

            
        /* set initial plugins from preferences */
        setInput(input);
        setMorph(morph);
        setActor(actor);

            
        /* create bitmap */
        final int W = 200;
        final int H = 200;
        mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.ARGB_8888);

        /* initialize the libvisual view */
        init(mBitmap);
    }

        
    @Override 
    protected void onDraw(Canvas canvas) 
    {
        /* render */
        //canvas.drawColor(0xFFCCCCCC);
        renderVisual(mBitmap);
        canvas.drawBitmap(mBitmap, 0, 0, null);
            
        /* force a redraw, with a different time-based pattern. */
        invalidate();
    }

    @Override
    public void onDetachedFromWindow()
    {
        /** deinitialize libvisual view */
        deinit();
        super.onDetachedFromWindow();
    }            

}
