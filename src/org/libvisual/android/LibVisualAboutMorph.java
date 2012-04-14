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


public class LibVisualAboutMorph extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.about_morph);

        /* get LinearLayout container */
        LinearLayout l = (LinearLayout) findViewById(R.id.layout_about_morph);

            
        /** set head */
        TextView head = new TextView(this);
        head.setTextAppearance(this, android.R.style.TextAppearance_Large);
        head.setTextColor(Color.WHITE);
        head.setText(LibVisual.curMorph.plugin.getPlugname()+" "+LibVisual.curMorph.plugin.getVersion());
        l.addView(head);


        /* description */
        TextView name = new TextView(this);
        name.setText(LibVisual.curMorph.plugin.getName());
        TextView desc = new TextView(this);
        desc.setText(LibVisual.curMorph.plugin.getAbout());
        l.addView(name);
        l.addView(desc);

        /* author */
        TextView title = new TextView(this);
        title.setTextAppearance(this, android.R.style.TextAppearance_WindowTitle);
        title.setPadding (0, 10, 0, 5);
        title.setText("Author:");
        l.addView(title);
        TextView author = new TextView(this);
        author.setText(LibVisual.curMorph.plugin.getAuthor());
        l.addView(author);

        /* help */
        title = new TextView(this);
        title.setTextAppearance(this, android.R.style.TextAppearance_WindowTitle);
        title.setPadding (0, 10, 0, 3);
        title.setText("Help:");
        l.addView(title);
        TextView help = new TextView(this);
        help.setText(LibVisual.curMorph.plugin.getHelp());
        l.addView(help);

        /* license */
        title = new TextView(this);
        title.setTextAppearance(this, android.R.style.TextAppearance_WindowTitle);
        title.setPadding (0, 10, 0, 5);
        title.setText("License:");
        l.addView(title);
        TextView license = new TextView(this);
        license.setText(LibVisual.curMorph.plugin.getLicense());
        l.addView(license);
    }
}

