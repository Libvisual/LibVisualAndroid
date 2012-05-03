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

/** PCM reader thread */

#include <unistd.h>
#include <android/log.h>
#include "AudioRecordJNI.h"

/** logging TAG */
#define TAG "AudioRecordThread"


jshortArray pcmA;
JNIEnv *env;



/** PCM read thread */
void AudioRecordThread_read(AudioRecordJNI r, jint samples, jint *running)
{

    /* get VM */
    JavaVM *vm = (*env)->GetJavaVM(env, &vm);
    
    /* attach thread */
    (*vm)->AttachCurrentThread(vm, &env, 0);

    /* create new shortArray */
    pcmA = (*env)->NewShortArray(env, samples);

    /* change thread priority */
    jclass ProcessClass;
    ProcessClass = (*env)->FindClass(env, "android/os/Process");
    jmethodID setThreadPriorityID = (*env)->GetStaticMethodID(env, ProcessClass, "setThreadPriority", "(I)V");
    (*env)->CallStaticIntMethod(env, ProcessClass, setThreadPriorityID, THREAD_PRIORITY_URGENT_AUDIO);

                
    /* wait until recorder is initialized */
    while(AudioRecord_getState(r) != STATE_INITIALIZED)
                usleep(10);
        
    AudioRecord_startRecording(r);

    /* wait until we're recording */
    while(AudioRecord_getRecordingState(r) != RECORDSTATE_RECORDING)
                usleep(10);
        
    while(*running)
    {
        /* read from recorder */
        jint input;
        if((input = AudioRecord_read(r, pcmA, (*env)->GetArrayLength(env, pcmA)/sizeof(jshort))) < 0)
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

        usleep(10);
    }
        
    /* exceptions? */
    if ((*env)->ExceptionOccurred(env)) 
    {
       (*env)->ExceptionDescribe(env);
       (*env)->ExceptionClear(env);
    }

    AudioRecord_stop(r);
        
    /* detach the current thread from the JVM. */
    (*vm)->DetachCurrentThread(vm);
}


/** lock PCM buffer */
jshort *AudioRecordThread_lockBuf()
{           
    return (*env)->GetShortArrayElements(env, pcmA, NULL);
}


/** unlock PCM buffer */
void AudioRecordThread_unlockBuf(jshort *buf)
{        
    (*env)->ReleaseShortArrayElements(env, pcmA, buf, JNI_ABORT);
}
