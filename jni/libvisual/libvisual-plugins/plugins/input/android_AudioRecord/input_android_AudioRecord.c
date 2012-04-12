/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2012
 *
 * Authors: Scott Sibley <sisibley@gmail.com>
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <math.h>
#include <jni.h>
#include <libvisual/libvisual.h>
#include "AudioRecordJNI.h"


/** buffersize in PCM samples */
#define MAX_PCM 4096


/** private parts */
typedef struct 
{
    AudioRecordJNI recorder;
    int16_t pcm_data[MAX_PCM];
    int size;
    VisAudioSampleRateType rate;
    VisAudioSampleChannelType channels;
    VisAudioSampleFormatType encoding;
} audioRecordPriv;



VISUAL_PLUGIN_API_VERSION_VALIDATOR




/******************************************************************************/



/** initialize */
int audioRecord_init (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

    /** allocate private structure */
    audioRecordPriv *priv;
	if(!(priv = visual_mem_new0(audioRecordPriv, 1)))
        return -1;
	visual_object_set_private (VISUAL_OBJECT(plugin), priv);

    priv->recorder = AudioRecord(MIC, 
                                 44100, 
                                 CHANNEL_IN_MONO, 
                                 ENCODING_PCM_16BIT, 
                                 MAX_PCM);
        
    return 0;
}


/** deinitialize */
int audioRecord_cleanup (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);


    /* free private structure */
    audioRecordPriv *priv;
    if((priv = visual_object_get_private (VISUAL_OBJECT (plugin))))
        visual_mem_free (priv);

    return 0;
}


/** libvisual calls this when it wants us to upload PCM data */
int audioRecord_upload(VisPluginData *plugin, VisAudio *audio)
{
    return 0;
}


/** hand info about us over to libvisual */
const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input = 
    {
            .upload = audioRecord_upload
    };

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "android_audioRecord",
        .name = "Android AudioRecord class wrapper",
        .author = "Daniel Hiepler <daniel@niftylight.de>",
        .version = "0.1",
        .about = ("Android input plugin for libvisual"),
        .help = ("You sing into the mic, and LibVisual goes nuts. Sing loud. With feeling."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,
        .init = audioRecord_init,
        .cleanup = audioRecord_cleanup,
        .plugin = VISUAL_OBJECT (&input)
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}
