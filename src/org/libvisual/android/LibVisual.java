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

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.util.Log;



public class LibVisual extends Activity
{
    private final static String TAG = "LibVisual";

        
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle state)
    {
        super.onCreate(state);
        setContentView(new LibVisualView(this));
    }
        
    /* load our native library */
    static 
    {
        try
        {
            System.loadLibrary("visual");
            System.loadLibrary("lvclient");
        }
        catch(UnsatisfiedLinkError e)
        {
            Log.e(TAG, "Error while loading shared library: "+e.getMessage());
            e.printStackTrace();
        }
    }
}


