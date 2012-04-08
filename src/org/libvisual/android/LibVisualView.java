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
    private long mStartTime;

    /* implementend by libplasma.so */
    private static native void renderVisual(Bitmap  bitmap, long time_ms);

    public LibVisualView(Context context) 
    {
        super(context);

        /* get settings */
        s = new LibVisualSettings(context);

        //String doMorphString = s.getString("prefs_do_morph", 
          //                      context.getString(R.string.default_do_morph));
        //Boolean doMorph = (doMorphString == "true");
        String input = s.getString("prefs_input", context.getString(R.string.default_plugin_input));
        String actor = s.getString("prefs_actor", context.getString(R.string.default_plugin_actor));
        String morph = s.getString("prefs_morph", context.getString(R.string.default_plugin_morph));
            
        Log.v(TAG, "Input: \""+input+"\" Morph: \""+morph+"\" Actor: \""+actor);
            
              
        /* create bitmap */
        final int W = 200;
        final int H = 200;
        mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);

        /* sample start time */
        mStartTime = System.currentTimeMillis();
    }

    @Override protected void onDraw(Canvas canvas) 
    {
        /* render */
        //canvas.drawColor(0xFFCCCCCC);
        renderVisual(mBitmap, System.currentTimeMillis() - mStartTime);
        canvas.drawBitmap(mBitmap, 0, 0, null);
            
        /* force a redraw, with a different time-based pattern. */
        invalidate();
    }

}
