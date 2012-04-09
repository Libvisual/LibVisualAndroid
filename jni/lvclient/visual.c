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
#include "visual.h"



/** local variables */
static struct
{

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

    LOGI("LibVisual.init(): %s", visual_get_version());

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
    visual_plugin_registry_add_path("/data/data/org.libvisual.android/lib");

        
    return JNI_TRUE;
}


/** LibVisual.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisual_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisual.deinit()");
        
    if(visual_is_initialized())
        visual_quit();
}


