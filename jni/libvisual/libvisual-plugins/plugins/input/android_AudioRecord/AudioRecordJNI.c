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


/** wrappers for AudioRecord class */

#include <unistd.h>
#include <android/log.h>
#include <libvisual/libvisual.h>
#include "AudioRecordJNI.h"

/** logging TAG */
#define TAG "AudioRecordJNI"


jclass AudioRecordClass;
jmethodID readID, releaseID, startRecordingID, stopID, getStateID, getRecordingStateID, getMinBufferSizeID;



/** create new recorder */
AudioRecordJNI AudioRecord(jint audioSource, 
                           jint sampleRateInHz, 
                           jint channelConfig, 
                           jint audioFormat, 
                           jint samples)
{


    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();
    
    /* get AudioRecord class */
    AudioRecordClass = (*env)->FindClass(env, "android/media/AudioRecord");
        
    /* static getMinBufferSize method */
    getMinBufferSizeID = (*env)->GetStaticMethodID(env, AudioRecordClass, "getMinBufferSize", "(III)I");

    /* constructor method */
    jmethodID initID = (*env)->GetMethodID(env, AudioRecordClass, "<init>", "(IIIII)V");

    /* release method */
    releaseID = (*env)->GetMethodID(env, AudioRecordClass, "release", "()V");
        
    /* read method */
    readID = (*env)->GetMethodID(env, AudioRecordClass, "read", "([SII)I");

    /* startRecording method */
    startRecordingID = (*env)->GetMethodID(env, AudioRecordClass, "startRecording", "()V");

    /* stop method */
    stopID = (*env)->GetMethodID(env, AudioRecordClass, "stop", "()V");

    /* getState method */
    getStateID = (*env)->GetMethodID(env, AudioRecordClass, "getState", "()I");

    /* getRecordingState method */
    getRecordingStateID = (*env)->GetMethodID(env, AudioRecordClass, "getRecordingState", "()I");
        
    /* create new instance */
    AudioRecordJNI r = (*env)->NewObject(env, AudioRecordClass, initID, audioSource, sampleRateInHz, channelConfig, audioFormat, samples);

        
    return r;
}


/** destroy recorder */
void AudioRecord_destroy(AudioRecordJNI r)
{
    if(!r)
	return;
	
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();

    /* release */
    (*env)->CallVoidMethod(env, r, releaseID);
        
    /* unref */
    (*env)->DeleteLocalRef(env, r);	
}


/** fill buffer with audio data */
jint AudioRecord_read(AudioRecordJNI r, jshortArray pcm, jint length)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    
                        
    return (*env)->CallIntMethod(env, r, readID, pcm, 0, length);
}


/** startRecording */
void AudioRecord_startRecording(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    

    (*env)->CallVoidMethod(env, r, startRecordingID);
}


/** stop */
void AudioRecord_stop(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    

    (*env)->CallVoidMethod(env, r, stopID);
}


/** getState */
jint AudioRecord_getState(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    
        
    return (*env)->CallIntMethod(env, r, getStateID);
}


/** getRecordingState */
jint AudioRecord_getRecordingState(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    
        
    return (*env)->CallIntMethod(env, r, getRecordingStateID);
}


/** getMinBufferSize */
jint AudioRecord_getMinBufferSize(jint sampleRateInHz, 
                                  jint channelConfig, 
                                  jint audioFormat)
{
    /* get environment */
    JNIEnv *env = visual_get_priv_ptr();    
        
    return (*env)->CallStaticIntMethod(env, AudioRecordClass, getMinBufferSizeID, sampleRateInHz, channelConfig, audioFormat);
}



