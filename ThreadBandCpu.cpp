/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "ThreadBandCpu.h"
/* Header for class ThreadBandCpu */
#ifdef __cplusplus
extern "C" {
#endif

struct ThreadParams
{
    JavaVM *jvm;
    jobject callback;
    int cpu;
};

void *myfun(void *ptr)
{
    ThreadParams* p = reinterpret_cast<ThreadParams*>(ptr);
    JavaVM *jvm = p->jvm;
    jobject callback = p->callback;
    int flag = p->cpu;
    free(p);

    if(flag >= 0) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(flag % sysconf(_SC_NPROCESSORS_CONF), &mask);
        if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
            fprintf(stderr, "set thread affinity failed\n");
        }
    }

    JNIEnv *env = NULL;
    jint res;
    res = jvm->AttachCurrentThread((void **)&env, NULL);
    if(res < 0) {
        fprintf(stderr, "Attach VM Thread failed\n");
        return NULL;
    }

    jclass RunnableInterface = env->GetObjectClass(callback);
    jmethodID Run = env->GetMethodID(RunnableInterface, "run","()V");
    env->CallVoidMethod(callback, Run);
    jvm->DetachCurrentThread();

    pthread_exit(NULL);
}
/*
 * Class:     ThreadBandCpu
 * Method:    bandCpu
 * Signature: (Ljava/lang/Runnable;I)V
 */
JNIEXPORT void JNICALL Java_ThreadBandCpu_bandCpu(JNIEnv *env, jclass cls, jobject obj, jint cpu) {
    pthread_t tid;
    ThreadParams* ptr = new ThreadParams();
    JavaVM *jvm = NULL;

    env->GetJavaVM(&jvm);
    ptr->callback = env->NewGlobalRef(obj);
    ptr->jvm = jvm;
    ptr->cpu = (int)cpu;

    if (pthread_create(&tid, NULL, myfun, reinterpret_cast<void*>(ptr)) != 0) {
        fprintf(stderr, "thread create failed\n");
        return ;
    }
    pthread_join(tid, NULL);
}

#ifdef __cplusplus
}
#endif