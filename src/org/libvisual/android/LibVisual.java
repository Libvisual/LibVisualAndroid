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
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;



public class LibVisual extends Activity
{
    private final static String TAG = "LibVisual";
        
    /** object to hold all our permanent settings */
    private static LibVisualSettings s;


        
        
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle state)
    {
        super.onCreate(state);

        /* create new settings-object */
        s = new LibVisualSettings(this);

        /* set our libvisual view */
        setContentView(new LibVisualView(this));
    }


    /** options menu */
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
            MenuInflater inflater = getMenuInflater();
            inflater.inflate(R.menu.libvisual, menu);
            return true;
    }

    
    /** item from options menu selected */
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
            switch (item.getItemId())
            {
                    /** About */
                    case R.id.about:
                    {
                            //startActivity(new Intent(this, AboutActivity.class));
                            return true;
                    }
                            
                    /** Preferences */
                    case R.id.settings:
                    {
                            startActivity(new Intent(this, LibVisualPreferences.class));
                            return true;
                    }

                    /** wtf? */
                    default:
                    {
                            Log.w(TAG, "Unhandled menu-item. This is a bug!");
                            break;
                    }
            }

            return false;
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


