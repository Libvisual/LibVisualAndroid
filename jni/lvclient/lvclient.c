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



/** VisLog -> android Log glue */
static void _log_handler(VisLogSeverity severity, const char *msg, const VisLogSource *source, void *priv)
{

    switch(severity)
    {
        case VISUAL_LOG_DEBUG:
            LOGI("lvDEBUG: (%s) line # %d (%s) : %s", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("lvINFO: %s: %s", __lv_progname, msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("lvWARNING: %s: %s", __lv_progname, msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("lvERROR: (%s) line # %d (%s) : %s", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("lvCRITICAL: (%s) line # %d (%s) : %s", source->file, source->line, source->func, msg);
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
        
    //visual_init_path_add("/data/data/org.libvisual.android/lib");
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
        
    return JNI_TRUE;
}


/** LibVisual.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisual_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisual.deinit()");
    visual_quit();
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

    return JNI_TRUE;
}


/** LibVisualView.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_deinit(JNIEnv * env, jobject  obj)
{

}


/** LibVisualView.renderVisual() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_renderVisual(JNIEnv * env, jobject  obj, jobject bitmap)
{
    void*              pixels;
    int                ret;
    static Stats       stats;
    static int         init;

    if (!init) 
    {
        /* initialize framerate stats */
        stats_init(&stats);
        init = 1;
    }

    
    /* lock bitmap for drawing */
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    /* start fps timing */
    stats_startFrame(&stats);

    /* Now fill the values with a nice little plasma */
    //fill_plasma(&info, pixels, time_ms );

    /* unlock bitmap */
    AndroidBitmap_unlockPixels(env, bitmap);

    /* stop fps timing */
    stats_endFrame(&stats);
}
