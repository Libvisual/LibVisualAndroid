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




/** VisBin wrapper */
public class VisBin
{
    public int VisBin;

    /** implemented by visual.c */
    private native int binNew();
    private native int binUnref(int binPtr);
    private native int binSetDepth(int binPtr, int depth);
    private native int binSetSupportedDepth(int binPtr, int depth);
    private native int binSetPreferredDepth(int binPtr, int depth);
    private native int binSetVideo(int binPtr, int videoPtr);
    private native int binRealize(int binPtr);
    private native int binSync(int binPtr, boolean noevent);
    private native int binDepthChanged(int binPtr);
    private native int binConnect(int binPtr, int actorPtr, int inputPtr);
    private native int binSetMorphByName(int binPtr, String name);
    private native int binSwitchActorByName(int binPtr, String name);
                
        
    public VisBin()
    {
        VisBin = binNew();
    }

    public void setDepth(int depth)
    {
        binSetDepth(VisBin, depth);
    }
        
    public void setSupportedDepth(int depth)
    {
        binSetSupportedDepth(VisBin, depth);
    }

    public void setPreferredDepth(int depth)
    {
        binSetPreferredDepth(VisBin, depth);
    }

    public void setVideo(int videoPtr)
    {
        binSetVideo(VisBin, videoPtr);
    }

    public void realize()
    {
        binRealize(VisBin);
    }

    public void sync(boolean noevent)
    {
        binSync(VisBin, noevent);
    }

    public void depthChanged()
    {
        binDepthChanged(VisBin);
    }

    public void connect(int actorPtr, int inputPtr)
    {
        binConnect(VisBin, actorPtr, inputPtr);
    }

    public void setMorph(String name)
    {
        binSetMorphByName(VisBin, name);
    }

    public void switchActor(String name)
    {
        binSwitchActorByName(VisBin, name);
    }
        
    @Override
    public void finalize()
    {
        binUnref(VisBin);
    }
}

