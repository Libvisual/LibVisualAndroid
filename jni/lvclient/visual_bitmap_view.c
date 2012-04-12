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
    /* our Bitmap buffer */
    VisVideo *bitmap;
    /* libvisual palette object */
    VisPalette *palette;
    /* libvisual bin object */
    VisBin *bin;
    /* plugin names */
    char *actor_name;
    char *input_name;
    char *morph_name;
}_v;


        

/** LibVisualBitmapView.setVideo(Bitmap) */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualBitmapView_setBitmap(JNIEnv * env, 
                                                                         jobject  obj, 
                                                                         jobject bitmap)
{
    LOGI("LibVisualView.setBitmap()");

    /* get BitmapInfo */
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) 
    {
        LOGE("AndroidBitmap_getInfo() failed");
        return JNI_FALSE;
    }

    /* validate format */
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) 
    {
        LOGE("Bitmap format is not RGBA_8888 !");
        return JNI_FALSE;
    }

        
    LOGI("Bitmap: %dx%d, stride: %d", info.width, info.height, info.stride);

        

        
    /* create VisVideo for our bitmap */
    VisVideo *b;
    if(!(b = visual_video_new()))
                return JNI_FALSE;
        
    visual_video_set_attributes(b,
                                info.width, info.height,
                                info.stride,
                                VISUAL_VIDEO_DEPTH_32BIT);


    /* do we have an actor? */
    if(!_v.bin || !_v.bin->actor)
    {
        LOGI("No active actor...");
        return JNI_FALSE;
    }

    /* get depth info from actor */
    VisVideoDepth depth;
    int depthflag = visual_actor_get_supported_depth(_v.bin->actor);
    if(depthflag == VISUAL_VIDEO_DEPTH_GL)
    {
        depth = visual_video_depth_get_highest(depthflag);
    }
    else
    {
        depth = visual_video_depth_get_highest_nogl(depthflag);
    }
    

    /* create video for actor */
    VisVideo *v = visual_video_new();
    visual_video_set_attributes(v, 
                                info.width, info.height, 
                                info.width*visual_video_bpp_from_depth(depth), 
                                depth);
    visual_video_allocate_buffer(v);


    /* connect new VisVideo to VisBin */
    visual_bin_set_video(_v.bin, v);
    visual_bin_realize(_v.bin);
    visual_bin_sync(_v.bin, FALSE);
    visual_bin_depth_changed(_v.bin);

        
    /* free current actor VisVideo? */
    if(_v.video)
    {
        visual_video_free_buffer(_v.video);
        visual_object_unref(VISUAL_OBJECT(_v.video));
        _v.video = NULL;
    }
        
    /* free old VisVideo ? */
    if(_v.bitmap)
    {
        visual_object_unref(VISUAL_OBJECT(_v.bitmap));
        _v.bitmap = NULL;
    }

        
    /* save VisVideos */
    _v.video = v;
    _v.bitmap = b;
        
    return JNI_TRUE;
}


/** LibVisualBitmapView.init() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualBitmapView_init(JNIEnv * env, 
                                                                         jobject  obj)
{
    LOGI("LibVisualView.init()");

    /* create new VisBin */
    if(!(_v.bin = visual_bin_new()))
                return JNI_FALSE;

    visual_bin_set_supported_depth(_v.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_preferred_depth(_v.bin, VISUAL_VIDEO_DEPTH_32BIT);
    /*visual_bin_switch_set_style(_v.bin, VISUAL_SWITCH_STYLE_DIRECT);
    visual_bin_switch_set_automatic(_v.bin, JNI_TRUE);
    visual_bin_switch_set_steps(_v.bin, 3);*/


    /* create actor */
    VisActor *a;
    if(!(a = visual_actor_new(_v.actor_name)))
                return JNI_FALSE;

    /* create input */
    VisInput *i;
    if(!(i = visual_input_new(_v.input_name)))
                return JNI_FALSE;

    /* get depth of actor */
    VisVideoDepth depth;
    int depthflag = visual_actor_get_supported_depth(a);
    if(depthflag == VISUAL_VIDEO_DEPTH_GL) 
    {
        depth = visual_video_depth_get_highest(depthflag);
    }
    else 
    {
        depth = visual_video_depth_get_highest_nogl(depthflag);
    }

    /* set bin to actor-depth */
    visual_bin_set_depth(_v.bin, depth);

        
    /* put everything together */
    visual_bin_connect(_v.bin, a, i);
        
    /* initialize framerate stats */
    fps_init(&_v.fps);

    return JNI_TRUE;
}


/** LibVisualBitmapView.deinit() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualBitmapView_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisualView.deinit()");

    if(_v.video)
    {
        visual_video_free_buffer(_v.video);
        visual_object_unref(VISUAL_OBJECT(_v.video));
        _v.video = NULL;
    }

    if(_v.bitmap)
    {
        visual_object_unref(VISUAL_OBJECT(_v.bitmap));
        _v.bitmap = NULL;
    }
        
    if(_v.bin)
    {
        visual_object_unref(VISUAL_OBJECT(_v.bin));
        _v.bin = NULL;
    }

    if(_v.palette)
    {
        visual_object_unref(VISUAL_OBJECT(_v.palette));
        _v.palette = NULL;
    }

    if(_v.actor_name)
    {
        free(_v.actor_name);
        _v.actor_name = NULL;
    }
    if(_v.morph_name)
    {
        free(_v.morph_name);
        _v.morph_name = NULL;
    }
    if(_v.input_name)
    {
        free(_v.input_name);
        _v.input_name = NULL;
    }

}


/** LibVisualBitmapView.setActor() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualBitmapView_setActor(JNIEnv *env, jobject  obj, jstring string)
{
    
    jboolean isCopy;  
    const char *actor = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting actor to \"%s\"", actor);

    /* actor valid ? */
    if(!(visual_plugin_registry_has_plugin(VISUAL_PLUGIN_TYPE_ACTOR, actor)))
    {
            LOGE("Invalid actor-plugin: \"%s\"", actor);
            return JNI_FALSE;
    }

    /* free previous actor name */
    if(_v.actor_name)
        free(_v.actor_name);

    /* save new actor name */
    _v.actor_name = strndup(actor, 128);
        
        
    (*env)->ReleaseStringUTFChars(env, string, actor);

    return JNI_TRUE;
}

/** LibVisualBitmapView.getActor() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualBitmapView_getActor(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.actor_name);
}


/** LibVisualBitmapView.setInput() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualBitmapView_setInput(JNIEnv *env, jobject  obj, jstring string)
{
    jboolean isCopy;  
    const char *input = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting input to \"%s\"", input);

    /* plugin valid ? */
    if(!(visual_plugin_registry_has_plugin(VISUAL_PLUGIN_TYPE_INPUT, input)))
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


/** LibVisualBitmapView.getInput() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualBitmapView_getInput(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.input_name);
}


/** LibVisualBitmapView.setMorph() */
JNIEXPORT jboolean JNICALL Java_org_libvisual_android_LibVisualBitmapView_setMorph(JNIEnv *env, jobject  obj, jstring string)
{
    jboolean isCopy;  
    const char *morph = (*env)->GetStringUTFChars(env, string, &isCopy);  

    LOGI("Setting morph to \"%s\"", morph);

    /* plugin valid ? */
    if(!(visual_plugin_registry_has_plugin(VISUAL_PLUGIN_TYPE_MORPH, morph)))
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


/** LibVisualBitmapView.getMorph() */
JNIEXPORT jstring JNICALL Java_org_libvisual_android_LibVisualBitmapView_getMorph(JNIEnv *env)
{
    return (*env)->NewStringUTF(env, _v.morph_name);
}


/** LibVisualBitmapView.renderVisual() */
JNIEXPORT void JNICALL Java_org_libvisual_android_LibVisualBitmapView_renderVisual(JNIEnv * env, jobject  obj, jobject bitmap)
{
    if(!visual_is_initialized() ||
       !_v.video ||
       !_v.bitmap ||
       !_v.bin || 
       !_v.bin->input || 
       !_v.bin->actor)
                return;

    /* start fps timing */
    fps_startFrame(&_v.fps);

    /* run libvisual pipeline */
    visual_bin_run(_v.bin);

        
    /* lock bitmap for drawing */
    int ret;
    void *pixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }


    /* set buffer to pixels */
    visual_video_set_buffer(_v.bitmap, pixels);
    
    /* depth transform */
    visual_video_depth_transform(_v.bitmap, _v.video);

        
    /* unlock bitmap */
    AndroidBitmap_unlockPixels(env, bitmap);

    /* stop fps timing */
    fps_endFrame(&_v.fps);
}
