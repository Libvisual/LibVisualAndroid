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
#include "AudioRecordJNI.h"

/** logging TAG */
#define TAG "AudioRecordJNI"


JavaVM *m_vm;
jclass AudioRecordClass;
jmethodID readID, releaseID, startRecordingID, stopID, getStateID, getRecordingStateID, getMinBufferSizeID;
jshortArray pcmA;



/** library loaded */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    /* save JavaVM */
    m_vm = vm;

    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);
        
    /* get AudioRecord class */
    AudioRecordClass = (*env)->FindClass(env, "android/media/AudioRecord");
        
    /* static getMinBufferSize method */
    getMinBufferSizeID = (*env)->GetStaticMethodID(env, AudioRecordClass, "getMinBufferSize", "(III)I");
    
    return JNI_VERSION_1_4;
}



/** create new recorder */
AudioRecordJNI AudioRecord(jint audioSource, 
                           jint sampleRateInHz, 
                           jint channelConfig, 
                           jint audioFormat, 
                           jint samples)
{

    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
    
    
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

    /* create new shortArray */
    pcmA = (*env)->NewShortArray(env, samples);
        
    return r;
}


/** destroy recorder */
void AudioRecord_destroy(AudioRecordJNI r)
{
    if(!r)
	return;
	
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    

    /* release */
    (*env)->CallVoidMethod(env, r, releaseID);
        
    /* unref */
    (*env)->DeleteLocalRef(env, r);	
}


/** fill buffer with audio data */
jint AudioRecord_read(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
        
    return (*env)->CallIntMethod(env, r, readID, pcmA, 0, (*env)->GetArrayLength(env, pcmA));
}


/** startRecording */
void AudioRecord_startRecording(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    

    (*env)->CallVoidMethod(env, r, startRecordingID);
}


/** stop */
void AudioRecord_stop(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    

    (*env)->CallVoidMethod(env, r, stopID);
}


/** getState */
jint AudioRecord_getState(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
        
    return (*env)->CallIntMethod(env, r, getStateID);
}


/** getRecordingState */
jint AudioRecord_getRecordingState(AudioRecordJNI r)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
        
    return (*env)->CallIntMethod(env, r, getRecordingStateID);
}


/** getMinBufferSize */
jint AudioRecord_getMinBufferSize(jint sampleRateInHz, 
                                  jint channelConfig, 
                                  jint audioFormat)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
        
    return (*env)->CallStaticIntMethod(env, AudioRecordClass, getMinBufferSizeID, sampleRateInHz, channelConfig, audioFormat);
}


/** PCM read thread */
void AudioRecord_readThread(AudioRecordJNI r, int *running)
{

    /* attach thread */
    JNIEnv *env;
    (*m_vm)->AttachCurrentThread(m_vm, &env, 0);

    /* change thread priority */
    /*jclass ProcessClass;
    ProcessClass = (*env)->FindClass(env, "android/os/Process");
    jmethodID setThreadPriorityID = (*env)->GetStaticMethodID(env, ProcessClass, "setThreadPriority", "(I)V");
    (*env)->CallStaticIntMethod(env, ProcessClass, setThreadPriorityID, THREAD_PRIORITY_URGENT_AUDIO);*/
        
    /* wait until recorder is initialized */
    while(AudioRecord_getState(r) != STATE_INITIALIZED)
                usleep(1000);
        
    AudioRecord_startRecording(r);

    /* wait until we're recording */
    while(AudioRecord_getRecordingState(r) != RECORDSTATE_RECORDING)
                usleep(1000);
        
    while(*running)
    {
        /* read from recorder */
        jint input;
        if((input = AudioRecord_read(r)) < 0)
        {
            switch(input)
            {
                    case ERROR_INVALID_OPERATION:
                    {
                        __android_log_print(ANDROID_LOG_ERROR, TAG, "AudioRecord.read() failed: ERROR_INVALID_OPERATION");
                        break;
                    }

                    case ERROR_BAD_VALUE:
                    {
                        __android_log_print(ANDROID_LOG_ERROR, TAG, "AudioRecord.read() failed: ERROR_BAD_VALUE");
                        break;
                    }
            }
            break;
        }

        usleep(1000);
    }
        
    /* exceptions? */
    if ((*env)->ExceptionOccurred(env)) 
    {
       (*env)->ExceptionDescribe(env);
       (*env)->ExceptionClear(env);
    }

    AudioRecord_stop(r);
        
    /* detach the current thread from the JVM. */
    (*m_vm)->DetachCurrentThread(m_vm);
}


/** lock PCM buffer */
jshort *AudioRecord_lockBuf()
{    
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);
        
    return (*env)->GetShortArrayElements(env, pcmA, NULL);
}


/** unlock PCM buffer */
void AudioRecord_unlockBuf(jshort *buf)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);
        
    (*env)->ReleaseShortArrayElements(env, pcmA, buf, 0);
}
