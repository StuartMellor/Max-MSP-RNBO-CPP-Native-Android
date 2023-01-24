#include <jni.h>
#include <string>
#include <functional>
#include "android/log.h"
#include "MSPLoader.h"

MSPLoader *mspLoader;
jclass mClass;
jobject mObject;
jmethodID mStaticmethodID, jDeclareSliders;
static JavaVM *g_jvm;

int cDelcareParams(std::string params[], int count, JavaVM *jvm) {
    // the jvm will need to be passed to the MSPLoader prior to this function call
    if (jvm != nullptr) {
        // TODO refactor this as a separate function
        JNIEnv *env;
        jint res = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
        // This could also potentially be an "interpret error" function ...
        if (res != JNI_OK) {
            if (res == JNI_EDETACHED) {
                __android_log_print(ANDROID_LOG_ERROR, "MyApp",
                                    "JVM has not been attached:");
                return -1;
            } else if (res == JNI_EVERSION) {
                __android_log_print(ANDROID_LOG_ERROR, "MyApp",
                                    "JNI version not supported for JVM:");
                return -2;
                // TODO Could possibly implement a fix here like try different jni versions..
            } else {
                __android_log_print(ANDROID_LOG_ERROR, "MyApp",
                                    "Something went wrong with the JVM:");
                return -3;
            }
        }
        if (env != nullptr) {
            // Result was okay and env can be used...
            __android_log_print(ANDROID_LOG_INFO, "MyApp",
                                "Successfully received %i parameters: %s", count,
                                params[0].c_str());
            jobjectArray paramArray = env->NewObjectArray(count, env->FindClass("java/lang/String"),
                                                          env->NewStringUTF(""));

            for (int i = 0; i < count; i++) {
                env->SetObjectArrayElement(paramArray, i, env->NewStringUTF(params[i].c_str()));
            }

            env->CallVoidMethod(mObject, jDeclareSliders, paramArray);
            return 0;
        } else {
            return -4;
            __android_log_print(ANDROID_LOG_ERROR, "MyApp",
                                "JNIEnv unavailable... unable to declare RNBO parameters.");
        }
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "MyApp",
                            "JVM reference is nullptr!\nUnable to declare RNBO parameters.");
        return -5;
    }
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_updateParam(JNIEnv *env,
                                                     jobject /* this */,
                                                     jstring name,
                                                     jfloat val) {
    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(name, &isCopy);

    mspLoader->updateParam(convertedValue, val);
}




extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rnbo_1test_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_init(
        JNIEnv *env,
        jobject thiz) {
    __android_log_print(ANDROID_LOG_DEBUG, "MyApp", "Init called!");


    jclass clazz = env->GetObjectClass(thiz);
    mClass = (jclass) env->NewGlobalRef(clazz);
    mObject = (jobject) env->NewGlobalRef(thiz);

    // Example for static function.
//    mStaticmethodID = env->GetStaticMethodID(mClass,
//                                             "setStaticMethodByNativeCode", "(I)V");

// Used example for non-static function
    jDeclareSliders = env->GetMethodID(mClass, "declareSliders",
                                       "([Ljava/lang/String;)V");

    // May not need to have jvm as global...
    jint rs = (*env).GetJavaVM(&g_jvm);
    assert (rs == JNI_OK);

    mspLoader = new MSPLoader();

    std::function<int(std::string[], int, JavaVM *)> cDecPsRef = &cDelcareParams;
    mspLoader->open();
    // TODO Add error checking here...
    // We don't want it to start if the UI isn't setup!
    mspLoader->init(cDelcareParams, g_jvm);
    mspLoader->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_destroy(
        JNIEnv *env,
        jobject /* this */) {

}








