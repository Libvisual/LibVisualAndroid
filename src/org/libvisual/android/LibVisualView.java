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



/** LibVisualView class to implement different drawing methods */
public class LibVisualView extends View 
{        
    private final static String TAG = "LibVisualView";

    Context ctxt;
    private LibVisualSettings s;


        
        
    /** constructor */
    public LibVisualView(Context context) 
    {
        super(context);

        /* save context */
        ctxt = context;
            
        /* get preferences */
        s = new LibVisualSettings(context);

        /* dim screen? */
        setScreenDimming();
    }

        
    /** set screen-dimming behaviour according to settings */
    public void setScreenDimming()
    {
        /* prevent dimming of screen? */
        String default_prevent_dimming = ctxt.getString(R.string.default_prevent_dimming);
        Boolean preventDimming = s.getBoolean("prefs_prevent_dimming",
                                                (default_prevent_dimming == "true" ? 
                                                          true : 
                                                          false));
        
        Log.v(TAG, "Prevent dimming: "+preventDimming);
                       
        /* don't dim screen */
        setKeepScreenOn(preventDimming);
    }
}
