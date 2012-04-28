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

JavaVM *m_vm;




/** library loaded */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    m_vm = vm;
        
    return JNI_VERSION_1_4;
}


/** create new recorder */
AudioRecordJNI AudioRecord(jint audioSource, 
                           jint sampleRateInHz, 
                           jint channelConfig, 
                           jint audioFormat, 
                           jint bufferSizeInBytes)
{
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    
    
    /* get AudioRecord class */
    jclass AudioRecordClass;
    AudioRecordClass = (*env)->FindClass(env, "android/media/AudioRecord");

    /* constructor method */
    jmethodID initID = (*env)->GetMethodID(env, AudioRecordClass, "<init>", "(IIIII)V");
    
    /* create new instance */
    return (*env)->NewObject(env, AudioRecordClass, initID, audioSource, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes);
}


/** destroy recorder */
void AudioRecord_destroy(AudioRecordJNI r)
{
    if(!r)
	return;
	
    /* get environment */
    JNIEnv *env;
    (*m_vm)->GetEnv(m_vm, (void **) &env, JNI_VERSION_1_4);    

    /* unref */
    (*env)->DeleteLocalRef(env, r);	
}
