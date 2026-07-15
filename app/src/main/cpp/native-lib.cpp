#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_studyoffline_NativeLib_getStatus(JNIEnv* env, jobject /* this */) {
    std::string status = "llama.cpp native bridge linked successfully";
    return env->NewStringUTF(status.c_str());
}