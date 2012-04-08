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
#include "fps.h"


/** local variables */
static struct
{
    /* used to count FPS */
    Fps fps;
    /* libvisual video object */
    VisVideo *video;
    /* libvisual palette object */
    VisPalette *palette;
    /* libvisual bin object */
    VisBin *bin;
    /* plugin names */
    char *actor_name;
    char *input_name;
    char *morph_name;
}_v;


        

/** LibVisualView.init() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualView_init(JNIEnv * env, jobject  obj, jobject bitmap)
{
    LOGI("LibVisualView.init()");

    /* get BitmapInfo */
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGE("AndroidBitmap_getInfo() failed");
        return JNI_FALSE;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return JNI_FALSE;
    }

    LOGI("Bitmap: %dx%d, stride: %d", info.width, info.height, info.stride);

        
    /* create new VisBin */
    if(!(_v.bin = visual_bin_new()))
                return JNI_FALSE;

    visual_bin_set_supported_depth(_v.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_preferred_depth(_v.bin, VISUAL_VIDEO_DEPTH_32BIT);

    /* create actor */
    VisActor *a;
    if(!(a = visual_actor_new(_v.actor_name)))
                return JNI_FALSE;

    /* create input */
    VisInput *i;
    if(!(i = visual_input_new(_v.input_name)))
                return JNI_FALSE;

    /* create video */
    _v.video = visual_video_new();
    visual_video_set_dimension(_v.video, info.width, info.height);
    visual_video_set_depth(_v.video, VISUAL_VIDEO_DEPTH_32BIT);
    visual_video_set_pitch(_v.video, info.stride);

    /* initialize framerate stats */
    fps_init(&_v.fps);
        
    return JNI_TRUE;
}


/** LibVisualView.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualView_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisualView.deinit()");

    if(_v.video)
    {
        visual_video_free_buffer(_v.video);
        visual_object_unref(VISUAL_OBJECT(_v.video));
        _v.video = NULL;
    }

    if(_v.bin)
    {
        if(_v.bin->actor)
            visual_object_unref(VISUAL_OBJECT(_v.bin->actor));      
            
        if(_v.bin->input)
            visual_object_unref(VISUAL_OBJECT(_v.bin->input));

        visual_object_unref(VISUAL_OBJECT(_v.bin));
        _v.bin = NULL;
    }

    if(_v.palette)
    {
        visual_object_unref(VISUAL_OBJECT(_v.palette));
        _v.palette = NULL;
    }

    if(_v.actor_name)
        free(_v.actor_name);
    if(_v.morph_name)
        free(_v.morph_name);
    if(_v.input_name)
        free(_v.input_name);
        
    if(visual_is_initialized())
        visual_quit();
}


/** LibVisualView.setActor() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualView_setActor(JNIEnv *env, jobject  obj, jstring string)
{
    
    jboolean isCopy;  
    const char *actor = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting actor to \"%s\"", actor);

    /* actor valid ? */
    /*if(!(visual_actor_is_valid_by_name(actor)))
    {
            LOGE("Invalid actor-plugin: \"%s\"", actor);
            return JNI_FALSE;
    }*/

    /* free previous actor name */
    if(_v.actor_name)
        free(_v.actor_name);

    /* save new actor name */
    _v.actor_name = strndup(actor, 128);
        
        
    (*env)->ReleaseStringUTFChars(env, string, actor);

    return JNI_TRUE;
}

/** LibVisualView.getActor() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualView_getActor(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.actor_name);
}


/** LibVisualView.setInput() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualView_setInput(JNIEnv *env, jobject  obj, jstring string)
{
    jboolean isCopy;  
    const char *input = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting input to \"%s\"", input);

    /* plugin valid ? */
    if(!(visual_input_valid_by_name(input)))
    {
            LOGE("Invalid input-plugin: \"%s\"", input);
            return JNI_FALSE;
    }

    /* free previous plugin name */
    if(_v.input_name)
        free(_v.input_name);

    /* save new plugin name */
    _v.input_name = strndup(input, 128);
        
        
    (*env)->ReleaseStringUTFChars(env, string, input);

    return JNI_TRUE;
}


/** LibVisualView.getInput() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualView_getInput(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.input_name);
}


/** LibVisualView.setMorph() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualView_setMorph(JNIEnv *env, jobject  obj, jstring string)
{
    jboolean isCopy;  
    const char *morph = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting morph to \"%s\"", morph);

    /* plugin valid ? */
    if(!(visual_morph_valid_by_name(morph)))
    {
            LOGE("Invalid morph-plugin: \"%s\"", morph);
            return JNI_FALSE;
    }

    /* free previous plugin name */
    if(_v.morph_name)
        free(_v.morph_name);

    /* save new plugin name */
    _v.morph_name = strndup(morph, 128);
        
        
    (*env)->ReleaseStringUTFChars(env, string, morph);

    return JNI_TRUE;
}


/** LibVisualView.getMorph() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualView_getMorph(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.morph_name);
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
    fps_startFrame(&_v.fps);

    /* Now fill the values with a nice little plasma */
    //fill_plasma(&info, pixels, time_ms );

    /* unlock bitmap */
    AndroidBitmap_unlockPixels(env, bitmap);

    /* stop fps timing */
    fps_endFrame(&_v.fps);
}

