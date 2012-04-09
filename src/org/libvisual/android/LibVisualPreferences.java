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

import android.appwidget.AppWidgetManager;
import android.content.Intent;
import android.preference.PreferenceActivity;
import android.os.Bundle;
import android.util.Log;


public class LibVisualPreferences extends PreferenceActivity
{
        private final static String TAG = "LibVisual/Preferences";
        
        
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
                super.onCreate(savedInstanceState);

                /** create prefs from xml */
                addPreferencesFromResource(R.xml.preferences);
        }

        /** another activity comes over this activity */
        @Override
        public void onPause()
        {
                /** update a wallpaper? */
                /*Intent i = new Intent(this, LibVisualWallpaper.class);
                i.setAction("com.rigid.birthdroid.PREFS_UPDATE");
                sendBroadcast(i);*/

                super.onPause();
        }
        
}

