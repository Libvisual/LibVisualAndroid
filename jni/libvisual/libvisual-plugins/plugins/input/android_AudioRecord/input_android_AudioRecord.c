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
#include <pthread.h>
#include <math.h>
#include <jni.h>
#include <android/log.h>
#include <libvisual/libvisual.h>
#include "AudioRecordJNI.h"


/** logging TAG */
#define TAG "input_android_AudioRecord"
/** buffersize in PCM samples */
#define MAX_PCM 2048


/** private parts */
typedef struct 
{
    AudioRecordJNI recorder;
    size_t bufsize;
    int recording;
    pthread_attr_t attr;
    pthread_t id;
} AudioRecordPriv;



VISUAL_PLUGIN_API_VERSION_VALIDATOR




/******************************************************************************/

#define LV_SAMPLERATE   VISUAL_AUDIO_SAMPLE_RATE_44100
#define LV_CHANNELS     VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO
#define LV_ENCODING     VISUAL_AUDIO_SAMPLE_FORMAT_S16
#define AND_SAMPLERATE  44100
#define AND_CHANNELS    CHANNEL_IN_MONO
#define AND_ENCODING    ENCODING_PCM_16BIT



/** PCM reader thread */
void *readThread(void *p)
{
    AudioRecordPriv *priv = p;
        
    /* record loop */
    AudioRecord_readThread(priv->recorder, &priv->recording);

    pthread_exit(0);
    
    return 0;
}


/** initialize */
int audioRecord_init (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

                    
    /** allocate private structure */
    AudioRecordPriv *priv;
    if(!(priv = visual_mem_new0(AudioRecordPriv, 1)))
        return -1;
        
    visual_object_set_private(VISUAL_OBJECT(plugin), priv);

    /* initialize threading */
    if(pthread_attr_init(&priv->attr) != 0)
        return -1;
        
    /* get minimum buffersize */
    priv->bufsize = AudioRecord_getMinBufferSize(AND_SAMPLERATE, 
                                                 AND_CHANNELS, 
                                                 AND_ENCODING);
    if(priv->bufsize == ERROR || priv->bufsize == ERROR_BAD_VALUE)
    {
        __android_log_print(ANDROID_LOG_INFO, TAG, "Error getting PCM bufsize");
        return -1;
    }
        
    __android_log_print(ANDROID_LOG_INFO, TAG, "Using buffersize 2*%d", priv->bufsize);

    /* use 2*minBufSize */
    priv->bufsize *= 2;
                
    /* create AudioRecorder */
    priv->recorder = AudioRecord(MIC, 
                                 AND_SAMPLERATE, 
                                 AND_CHANNELS, 
                                 AND_ENCODING, 
                                 priv->bufsize/sizeof(jshort));        

    /* start recording */
    priv->recording = TRUE;

            
    /* start reader thread */
    if(pthread_create(&priv->id, &priv->attr, readThread, priv) != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "pthread_create() failed");
        return -1;
    }
        
    return 0;
}


/** deinitialize */
int audioRecord_cleanup (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);


    /* get private structure */
    AudioRecordPriv *priv;
    if(!(priv = visual_object_get_private (VISUAL_OBJECT (plugin))))
        return 0;
    
    /* stop recording */
    priv->recording = FALSE;

    /* deinitialize pthreads */
    pthread_attr_destroy(&priv->attr);
        
    /* join thread */
    pthread_join(priv->id, 0);
        
    /* destroy recorder */
    AudioRecord_destroy(priv->recorder);

    /* free private descriptor */
    visual_mem_free (priv);

    return 0;
}


/** libvisual calls this when it wants us to upload PCM data */
int audioRecord_upload(VisPluginData *plugin, VisAudio *audio)
{
    AudioRecordPriv *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    

    int16_t *buf = AudioRecord_lockBuf();
                        
    VisBuffer *buffer = visual_buffer_new_wrap_data(buf, priv->bufsize);
                                                                                         
    visual_audio_samplepool_input(audio->samplepool, 
                                  buffer, 
                                  LV_SAMPLERATE,
                                  LV_ENCODING, 
                                  LV_CHANNELS);
                                                                 
    visual_buffer_free(buffer);

    AudioRecord_unlockBuf(buf);
                                                                                                                                                                                                                                                                                                                                  
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
