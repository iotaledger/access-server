/*
* This file is part of the Frost distribution
* (https://github.com/xainag/frost)
*
* Copyright (c) 2019 XAIN AG.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <jni.h>
#include <string>
#include <android/log.h>
#include <libauthdac.h>

// Android log function wrappers
static const char* kTAG = "jniAPILibDacAuthNative";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

// processing callback to handler class
typedef struct JniLibDacAuthContext {
    jclass   libDacAuthClz;
    jobject  libDacAuthObj;
    jobject  socketObj;

} JniLibDacAuthContext_t;

JavaVM* javaVM = NULL;

ssize_t read_socket(void *ext, void *data, unsigned short len)
{
    JniLibDacAuthContext_t *ctx = (JniLibDacAuthContext_t *)ext;
    JNIEnv *env;

    LOGI("read_socket");

    javaVM->AttachCurrentThread(&env, NULL);

    jmethodID receiveData = env->GetMethodID(ctx->libDacAuthClz, "ReceiveData",
            "(Lcom/de/xain/emdac/api/tcp/TCPSocketObject;[BS)I");

    if (receiveData == NULL || env->ExceptionOccurred()) {
        env->ExceptionClear();
        // do smth in case of failure
        LOGE("read_socket sendData");
        return 0;
    }


    LOGI("read_socket");

    jbyteArray data_ = env->NewByteArray(len);

    jint ret = env->CallIntMethod(ctx->libDacAuthObj, receiveData, ctx->socketObj, data_, len);

    env->GetByteArrayRegion(data_, 0, len, (jbyte *)data);

    env->DeleteLocalRef(data_);
    LOGI("read_socket %d", ret);

    return ret;
}

ssize_t write_socket(void *ext, void *data, unsigned short len)
{
    JniLibDacAuthContext_t *ctx = (JniLibDacAuthContext_t *)ext;
    JNIEnv *env;

    LOGI("write_socket");

    javaVM->AttachCurrentThread(&env, NULL);

    jmethodID sendData = env->GetMethodID(ctx->libDacAuthClz, "SendData",
            "(Lcom/de/xain/emdac/api/tcp/TCPSocketObject;[BS)I");

    if (sendData == NULL || env->ExceptionOccurred()) {
        env->ExceptionClear();
        // do smth in case of failure
        LOGE("write_socket receiveData");
        return 0;
    }

    LOGI("write_socket");

    jbyteArray data_ = env->NewByteArray(len);
    env->SetByteArrayRegion (data_, 0, len, (const jbyte *)data);

    jint ret = env->CallIntMethod(ctx->libDacAuthObj, sendData, ctx->socketObj, data_, len);

    env->DeleteLocalRef(data_);
    LOGI("write_socket %d", ret);

    return ret;
}

int verify(unsigned char *key, int len)
{
    return 0;//DAC_OK;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacInitClient(JNIEnv *env, jobject instance,
                                                                   jlongArray session_, jobject socketObj) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    jint ret = 0;

    env->GetJavaVM(&javaVM);

    dacSession_t *dacSession = (dacSession_t *)malloc(sizeof(dacSession_t));
    JniLibDacAuthContext_t *jniLibDacAuthContext = (JniLibDacAuthContext_t *)malloc(sizeof(JniLibDacAuthContext_t));

    ret = (jint) dacInitClient(dacSession, jniLibDacAuthContext);
    dacSession->f_read = read_socket;
    dacSession->f_write = write_socket;
    dacSession->f_verify = verify;

    session[0] = (jlong) dacSession;

    jniLibDacAuthContext->libDacAuthClz = (jclass) env->NewGlobalRef(env->FindClass("com/de/xain/emdac/api/APILibDacAuthNative"));
    jniLibDacAuthContext->libDacAuthObj = env->NewGlobalRef(instance);
    jniLibDacAuthContext->socketObj = env->NewGlobalRef(socketObj);

    LOGI("APILibDacAuthNative_dacInitClient ret %d", ret);

    env->ReleaseLongArrayElements(session_, session, 0);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacAuthenticate(JNIEnv *env, jobject instance,
                                                                     jlongArray session_) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    dacSession_t *dacSession = (dacSession_t *) *session;
    jint ret = 0;

    ret = dacAuthenticate(dacSession);

    LOGI("APILibDacAuthNative_dacAuthenticate ret %d", ret);

    env->ReleaseLongArrayElements(session_, session, 0);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacSetOption(JNIEnv *env, jobject instance,
                                                                  jlongArray session_, jstring key_,
                                                                  jbyteArray data_) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    const char *key = env->GetStringUTFChars(key_, 0);
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    dacSession_t *dacSession = (dacSession_t *) *session;
    jint ret = 0;

    // TODO: Write this
    dacSetOption(dacSession, key, (unsigned char *)data);
    LOGI("APILibDacAuthNative_dacSetOption ret %d", ret);

    env->ReleaseLongArrayElements(session_, session, 0);
    env->ReleaseStringUTFChars(key_, key);
    env->ReleaseByteArrayElements(data_, data, 0);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacSend(JNIEnv *env, jobject instance,
                                                             jlongArray session_, jbyteArray data_,
                                                             jshort length) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    dacSession_t *dacSession = (dacSession_t *) *session;
    jint ret = 0;

    // TODO: test this

    LOGI("APILibDacAuthNative_dacSend");
    ret = dacSend(dacSession, (const unsigned char *)data, length);

    env->ReleaseByteArrayElements(data_, data, 0);
    env->ReleaseLongArrayElements(session_, session, 0);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacReceive(JNIEnv *env, jobject instance,
                                                                jlongArray session_, jbyteArray data_,
                                                                jshort length) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    dacSession_t *dacSession = (dacSession_t *) *session;
    jint ret = 0;

    // TODO: test this

    LOGI("APILibDacAuthNative_dacReceive");
    ret = dacReceive(dacSession, (unsigned char **)&data, (unsigned short*)&length);

    env->ReleaseLongArrayElements(session_, session, 0);
    env->ReleaseByteArrayElements(data_, data, 0);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_de_xain_emdac_api_APILibDacAuthNative_dacRelease(JNIEnv *env, jobject instance,
                                                                jlongArray session_) {
    if (session_ == NULL) {
        return 0;
    }
    jlong *session = env->GetLongArrayElements(session_, NULL);
    dacSession_t *dacSession = (dacSession_t *) *session;
    jint ret = 0;

    // TODO: release object references
    ret = dacRelease(dacSession);

    LOGI("APILibDacAuthNative_dacRelease");

    env->ReleaseLongArrayElements(session_, session, 0);
    return ret;
}
