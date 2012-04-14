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
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.util.TypedValue;
import android.widget.LinearLayout;
import android.widget.TextView;


public class LibVisualAboutActor extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.about_actor);

        /* get LinearLayout container */
        LinearLayout l = (LinearLayout) findViewById(R.id.layout_about_actor);

            
        /** set head */
        TextView head = new TextView(this);
        head.setTypeface(Typeface.MONOSPACE, Typeface.BOLD);
        head.setTextSize(TypedValue.COMPLEX_UNIT_SP, 25);
        head.setTextColor(Color.WHITE);
        head.setText(LibVisual.curActor.plugin.getPlugname()+" "+LibVisual.curActor.plugin.getVersion());
        l.addView(head);

        /* description */
        TextView name = new TextView(this);
        name.setText(LibVisual.curActor.plugin.getName());
        TextView desc = new TextView(this);
        desc.setText(LibVisual.curActor.plugin.getAbout());
        l.addView(name);
        l.addView(desc);

        /* author */
        TextView author = new TextView(this);
        author.setText(LibVisual.curActor.plugin.getAuthor());
        l.addView(author);

        /* help */
        TextView help = new TextView(this);
        help.setText(LibVisual.curActor.plugin.getHelp());
        l.addView(help);

        /* license */
        TextView license = new TextView(this);
        license.setText(LibVisual.curActor.plugin.getLicense());
        l.addView(license);
    }
}

