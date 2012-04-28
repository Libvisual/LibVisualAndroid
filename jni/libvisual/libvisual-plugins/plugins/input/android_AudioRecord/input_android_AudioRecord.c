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
    
    float frequency;
    float ampltitude;
    float angle;
    float angle_step;                                
} AudioRecordPriv;




VISUAL_PLUGIN_API_VERSION_VALIDATOR




/******************************************************************************/



/** initialize */
int audioRecord_init (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

    /** allocate private structure */
    AudioRecordPriv *priv;
    if(!(priv = visual_mem_new0(AudioRecordPriv, 1)))
        return -1;
        
    visual_object_set_private (VISUAL_OBJECT(plugin), priv);


    priv->recorder = AudioRecord(MIC, 
                                 44100, 
                                 CHANNEL_IN_MONO, 
                                 ENCODING_PCM_16BIT, 
                                 MAX_PCM);

    
#define OUTPUT_RATE       44100
#define OUTPUT_SAMPLES    4096
#define DEFAULT_FREQUENCY (OUTPUT_RATE/25)
#define DEFAULT_AMPLITUDE 1.0
        
    priv->frequency  = DEFAULT_FREQUENCY;
    priv->ampltitude = DEFAULT_AMPLITUDE;
    priv->angle = 0.0;
    priv->angle_step = (2 * VISUAL_MATH_PI * priv->frequency) / OUTPUT_RATE;
                    

    return 0;
}


/** deinitialize */
int audioRecord_cleanup (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);


    /* free private structure */

    AudioRecordPriv *priv;
    if((priv = visual_object_get_private (VISUAL_OBJECT (plugin))))
        visual_mem_free (priv);

    /* destroy recorder */
    AudioRecord_destroy(priv->recorder);
    
    return 0;
}


/** libvisual calls this when it wants us to upload PCM data */
int audioRecord_upload(VisPluginData *plugin, VisAudio *audio)
{
    AudioRecordPriv *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    int16_t data[OUTPUT_SAMPLES];
    int i;
         
    for(i = 0; i < VISUAL_TABLESIZE(data); i++) 
    {
	data[i] = (int16_t) (65535/2 * priv->ampltitude * sin (priv->angle));
                             
        priv->angle += priv->angle_step;
        if(priv->angle >= 2 * VISUAL_MATH_PI) 
        {
    	    priv->angle -= 2 * VISUAL_MATH_PI;
        }
    }
                                                                                                                 
    VisBuffer *buffer = visual_buffer_new_wrap_data (data, VISUAL_TABLESIZE (data));
                                                                                         
    visual_audio_samplepool_input (audio->samplepool, buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
    VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
                                                                 
    visual_buffer_free(buffer);
                                                                                                                                                                                                                                                                                                                                  
    return 0;
}

/** hand over info about us over to libvisual */
const VisPluginInfo *get_plugin_info (void)
{
    static VisInputPlugin input = 
    {
        .upload = audioRecord_upload        
    };
                                
    static VisPluginInfo info = 
    {
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "android_AudioRecord",
        .name = "Android AudioRecord class wrapper",
        .author = "Daniel Hiepler <daniel@niftylight.de>",
        .version = "0.1",
        .about = N_("Android input plugin for libvisual"),
        .help = N_("You sing into the mic, and LibVisual goes nuts. Sing loud. With feeling."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,
        .init = audioRecord_init,
        .cleanup = audioRecord_cleanup,
        .plugin = VISUAL_OBJECT (&input)
    };
    
    return &info;
}
