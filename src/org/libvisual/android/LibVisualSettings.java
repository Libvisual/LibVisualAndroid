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
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.util.Log;



public class LibVisualSettings
{
        private final static String TAG = "LibVisual/Settings";
        
        /** our context */
        Context _c;
        /** our preferences */
        private SharedPreferences _prefs;
        private SharedPreferences.Editor _edit;



        /** constructor */
        public LibVisualSettings(Context c)
        {
                /** save context */
                _c = c;
                
                /** read prefs */
                //_prefs = PreferenceManager.getDefaultSharedPreferences(c);
                _prefs = c.getSharedPreferences(c.getPackageName()+"_preferences", 
                                                Context.MODE_PRIVATE);
                _edit = _prefs.edit();
                commit();
        }


        /** commit recent edits */
        public void commit()
        {
                /* commit edits */
                _edit.commit();
        }

        
        /** getter */
        String getString(String key, String defValue)
        {
                return _prefs.getString(key, defValue);
        }

        
        int getInt(String key, int defValue)
        {
                return _prefs.getInt(key, defValue);
        }

        
        boolean getBoolean(String key, boolean defValue)
        {
                return _prefs.getBoolean(key, defValue);
        }

        
        /** setter */
        void putString(String key, String value)
        {
                _edit.putString(key, value);
        }

        
        void putInt(String key, int value)
        {
                _edit.putInt(key, value);
        }


        void putBoolean(String key, boolean value)
        {
                _edit.putBoolean(key, value);
        }
}
