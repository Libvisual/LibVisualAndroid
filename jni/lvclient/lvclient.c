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
            LOGI("lvDEBUG: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("lvINFO: %s: %s\n", __lv_progname, msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("lvWARNING: %s: %s\n", __lv_progname, msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("lvERROR: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("lvCRITICAL: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
    }
}


/******************************************************************************
 ******************************************************************************/

/** LibVisual.init() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisual_init(JNIEnv * env, jobject  obj)
{
    if(visual_is_initialized())
                return;
        
    /*visual_init_path_add("/data/data/org.libvisual.android/lib");
    visual_log_set_verbosity(VISUAL_LOG_DEBUG);
    visual_log_set_handler(VISUAL_LOG_DEBUG, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_INFO, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_WARNING, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_CRITICAL, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_ERROR, _log_handler, NULL);

    visual_init (0, NULL);*/
}


/** LibVisualView.init() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_init(JNIEnv * env, jobject  obj, jobject bitmap)
{

}

/** LibVisualView.renderVisual() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_renderVisual(JNIEnv * env, jobject  obj, jobject bitmap)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    static int         init;

    if (!init) {
        //init_tables();
        stats_init(&stats);
        init = 1;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    stats_startFrame(&stats);

    /* Now fill the values with a nice little plasma */
    //fill_plasma(&info, pixels, time_ms );

    AndroidBitmap_unlockPixels(env, bitmap);

    stats_endFrame(&stats);
}
