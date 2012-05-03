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
import android.content.res.Configuration;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.widget.TextView;


/** 
 * @todo
 * mic input
 * scaling
 * actor/input/morph classes
 * GL View
 */

public class LibVisual extends Activity
{
    public static VisActor curActor;
    public static VisInput curInput;
    public static VisMorph curMorph;
    public static VisBin curBin;
        
    private final static String TAG = "LibVisual";
        
    private LibVisualSettings s;
    private LibVisualView v;
    private String defaultDoMorph;
    private String defaultMorph;
    private String defaultActor;
    private String defaultInput;
    private Boolean doMorph;
        
    /* implementend by liblvclient.so */
    private static native boolean init();
    private static native void deinit();


        
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle state)
    {
        super.onCreate(state);

        /* hide titlebar */
        getWindow().requestFeature(Window.FEATURE_NO_TITLE);

        /* create new settings-object */
        s = new LibVisualSettings(this);
                        
        /* initialize libvisual */
        init();

        /* do morph when changing actors? */
        defaultDoMorph = getString(R.string.default_do_morph);
        doMorph = s.getBoolean("prefs_do_morph", (defaultDoMorph == "true" ? 
                                                          true : 
                                                          false));

        /* morph plugin */
        defaultMorph = getString(R.string.default_plugin_morph);
        String prefMorph = s.getString("prefs_morph", defaultMorph);
        curMorph = new VisMorph(prefMorph);
            
        /* input plugin */
        defaultInput = getString(R.string.default_plugin_input);
        String prefInput = s.getString("prefs_input", defaultInput);
        curInput = new VisInput(prefInput);
            
        /* actor plugin */
        defaultActor = getString(R.string.default_plugin_actor);
        String prefActor = s.getString("prefs_actor", defaultActor);
        curActor = new VisActor(prefActor);

        /* create bin */
        curBin = new VisBin();
            
        /* set depth of bin */
        curBin.setSupportedDepth(VisVideo.VISUAL_VIDEO_DEPTH_ALL);
        curBin.setPreferredDepth(VisVideo.VISUAL_VIDEO_DEPTH_32BIT);
            
        /* create new LibVisualBitmapView */
        v = new LibVisualBitmapView(this);

        /* connect actor & input to bin */
        curBin.connect(curActor, curInput);
            
        /* set our LibVisualBitmapView as contentView */
        setContentView(v);
            
    }
        

    /** called by OS when this activity becomes active again */
    @Override
    public void onResume()
    {
            super.onResume();
            Log.i(TAG, "onResume()");

            /* screen dimming */
            v.setScreenDimming();

            
            /* do-morph */
            doMorph = s.getBoolean("prefs_do_morph", (defaultDoMorph == "true" ? 
                                                          true : 
                                                          false));
            
            /* morph changed? */
            String newMorphName = s.getString("prefs_morph", defaultMorph);
            if(!curMorph.plugin.getPlugname().equals(newMorphName))
            {
                Log.i(TAG, "Changing morph (\""+curMorph.plugin.getPlugname()+"\" -> \""+newMorphName+"\")");
                curBin.setMorph(newMorphName);
                curMorph = curBin.getMorph();
            }
            
            /* actor changed? */
            String newActorName = s.getString("prefs_actor", defaultActor);
            if(!curActor.plugin.getPlugname().equals(newActorName))
            {
                Log.i(TAG, "Changing actor (\""+curActor.plugin.getPlugname()+"\" -> \""+newActorName+"\")");
                    
                /* do morph? */
                if(doMorph)
                {
                        /* enable morph */
                }
                else
                {
                        /* disable morph */
                }

                /* switch actor */
                curBin.switchActor(newActorName);

                /* get new actor */
                curActor = curBin.getActor();
            }
            

            /* input changed? */
            String newInputName = s.getString("prefs_input", defaultInput);
            if(!curInput.plugin.getPlugname().equals(newInputName))
            {
                Log.i(TAG, "Changing input (\""+curInput.plugin.getPlugname()+"\" -> \""+newInputName+"\")");

                curInput = new VisInput(newInputName);

                /* connect actor & input to bin */
                curBin.connect(curActor, curInput);
            }
    }

    /** orientation changed */
    @Override
    public void onConfigurationChanged(Configuration newConfig) 
    {
        super.onConfigurationChanged(newConfig);
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
            /* About App */
            case R.id.about_libvisual_android:
            {
                startActivity(new Intent(this, LibVisualAboutApp.class));
                return true;
            }

            /* About Lib */
            case R.id.about_libvisual:
            {
                startActivity(new Intent(this, LibVisualAboutLib.class));
                return true;
            }

            /* About actor */
            case R.id.about_actor:
            {                                
                startActivity(new Intent(this, LibVisualAboutActor.class));
                return true;
            }

            /* About input */
            case R.id.about_input:
            {
                startActivity(new Intent(this, LibVisualAboutInput.class));
                return true;
            }

            /* About morph */
            case R.id.about_morph:
            {
                startActivity(new Intent(this, LibVisualAboutMorph.class));
                return true;
            }
                    
            /* Preferences */
            case R.id.settings:
            {
                    startActivity(new Intent(this, LibVisualPreferences.class));
                    return true;
            }

            /* wtf? */
            default:
            {
                    Log.w(TAG, "Unhandled menu-item. This is a bug!");
                    break;
            }
        }

        return false;
    }


    /* called when activity is destroyed */
    @Override
    public void onDestroy()
    {
        /* deinitialize libvisual */
        deinit();
            
        super.onDestroy();
    }

        

        
    /* load our native library */
    static 
    {
        try
        {
            System.loadLibrary("gnustl_shared");
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



