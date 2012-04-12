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

#include <android/log.h>
#include "AudioRecordJNI.h"

static JavaVM *_vm;
static JNIEnv *_env;
static jclass *AudioRecordClass;





/** create new recorder */
AudioRecordJNI AudioRecord(jint audioSource, 
                           jint sampleRateInHz, 
                           jint channelConfig, 
                           jint audioFormat, 
                           jint bufferSizeInBytes)
{
    jmethodID initID = (*_env)->GetMethodID(_env, AudioRecordClass, "<init>", "(Ljava/lang/int;Ljava/lang/int;Ljava/lang/int;Ljava/lang/int;Ljava/lang/int;)V;");
    return (*_env)->NewObject(_env, AudioRecordClass, initID, audioSource, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes);
}


/** library loaded */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    _vm = vm;

    /*if((*_vm)->GetEnv(_vm, (void**) &_env, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_ERROR, "input_AudioRecord", "Failed to get the environment using GetEnv()");
        return -1;
    }
        
    if(!(AudioRecordClass = (*_env)->FindClass(_env, "android/media/AudioRecord")))
    {
        __android_log_print(ANDROID_LOG_ERROR, "input_AudioRecord", "Failed to get AudioRecord class using FindClass()");
        return -1;
    }*/
        
    return JNI_VERSION_1_4;
}

