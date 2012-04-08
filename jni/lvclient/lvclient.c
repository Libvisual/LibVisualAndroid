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

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libvisual/libvisual.h>
#include "lvclient.h"
#include "stats.h"


/** local variables */
static struct
{
    /* used to count FPS */
    Stats fps_stats;
    /* libvisual video object */
    VisVideo *video;
    /* libvisual palette object */
    VisPalette *palette;
    /* libvisual bin object */
    VisBin *bin;
    /* plugin names */
    const char *actor_name;
    const char *input_name;
    const char *morph_name;
}_l;



/** VisLog -> android Log glue */
static void _log_handler(VisLogSeverity severity, const char *msg, const VisLogSource *source, void *priv)
{

    switch(severity)
    {
        case VISUAL_LOG_DEBUG:
            LOGI("(debug) %s(): %s", source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("(info) %s", msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("(WARNING) %s", msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("(ERROR) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("(CRITICAL) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
    }
}


/******************************************************************************
 ******************************************************************************/

/** LibVisual.init() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisual_init(JNIEnv * env, jobject  obj)
{
    if(visual_is_initialized())
                return JNI_TRUE;

    LOGI("LibVisual.init()");

#ifndef NDEBUG
    /* endless loop to wait for debugger to attach */
    int foo = 1;
    while(foo);
#endif
        
    /* register VisLog handler to make it log to android logcat */
    visual_log_set_handler(VISUAL_LOG_DEBUG, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_INFO, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_WARNING, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_CRITICAL, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_ERROR, _log_handler, NULL);
    visual_log_set_verbosity(VISUAL_LOG_DEBUG);

    /* initialize libvisual */
    char *v[] = { "lvclient", NULL };
    char **argv = v;
    int argc=1;
    visual_init(&argc,  &argv);

    /* add our plugin search path */
    visual_init_path_add("/data/data/org.libvisual.android/lib");
        
    /* default plugins */
    _l.actor_name = DEFAULT_ACTOR;
    _l.input_name = DEFAULT_INPUT;
    _l.morph_name = DEFAULT_MORPH;

        
    return JNI_TRUE;
}


/** LibVisual.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisual_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisual.deinit()");
}


/** LibVisualView.init() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualView_init(JNIEnv * env, jobject  obj, jobject bitmap)
{
    LOGI("LibVisualView.init()");

    /* get BitmapInfo */
    AndroidBitmapInfo  info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGE("AndroidBitmap_getInfo() failed");
        return JNI_FALSE;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return JNI_FALSE;
    }

    /* create new VisBin */
    if(!(_l.bin = visual_bin_new()))
                return JNI_FALSE;

    visual_bin_set_supported_depth(_l.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_preferred_depth(_l.bin, VISUAL_VIDEO_DEPTH_32BIT);

        
    /* initialize framerate stats */
    stats_init(&_l.fps_stats);
        
    return JNI_TRUE;
}


/** LibVisualView.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisualView.deinit()");

    if(_l.video)
    {
        visual_video_free_buffer(_l.video);
        visual_object_unref(VISUAL_OBJECT(_l.video));
        _l.video = NULL;
    }

    if(_l.bin)
    {
        if(_l.bin->actor)
            visual_object_unref(VISUAL_OBJECT(_l.bin->actor));      
            
        if(_l.bin->input)
            visual_object_unref(VISUAL_OBJECT(_l.bin->input));

        visual_object_unref(VISUAL_OBJECT(_l.bin));
        _l.bin = NULL;
    }

    if(_l.palette)
    {
        visual_object_unref(VISUAL_OBJECT(_l.palette));
        _l.palette = NULL;
    }
        
    if(visual_is_initialized())
        visual_quit();
}


/** LibVisualView.renderVisual() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_renderVisual(JNIEnv * env, jobject  obj, jobject bitmap)
{
    void*              pixels;
    int                ret;
    static int         init;

    if(!visual_is_initialized())
                return;
        
    
    /* lock bitmap for drawing */
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    /* start fps timing */
    stats_startFrame(&_l.fps_stats);

    /* Now fill the values with a nice little plasma */
    //fill_plasma(&info, pixels, time_ms );

    /* unlock bitmap */
    AndroidBitmap_unlockPixels(env, bitmap);

    /* stop fps timing */
    stats_endFrame(&_l.fps_stats);
}
