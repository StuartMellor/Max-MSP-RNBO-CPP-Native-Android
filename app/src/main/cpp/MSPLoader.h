//
// Created by stuar on 23/01/2023.
//

#ifndef RNBO_TEST_MSPLOADER_H
#define RNBO_TEST_MSPLOADER_H

#include "rnbo/RNBO.h"
#include <oboe/Oboe.h>
#include <jni.h>
#include <string>


class MSPLoader : public oboe::AudioStreamDataCallback {
public:
    oboe::Result open();
    oboe::Result start();
    oboe::Result stop();
    oboe::Result close();

    class MyErrorCallback : public oboe::AudioStreamErrorCallback {
    public:
        MyErrorCallback(MSPLoader *parent) : mParent(parent) {}
        virtual ~MyErrorCallback() {
        }
        void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
    private:
        MSPLoader *mParent;
    };
    int init(std::function<int(std::string[], int, JavaVM*)> cDecPsRef, JavaVM* jvm);
    static constexpr int kChannelCount = 1;
    void updateParam(const char *paramName, float val);
private:
    RNBO::PlatformInterface *platformInterface;
    RNBO::CoreObject rnboObject;
    std::shared_ptr<oboe::AudioStream> mStream;
    std::shared_ptr<MyErrorCallback> mErrorCallback;
    // Inherited from oboe::AudioStreamDataCallback
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames)
    override;
    RNBO::SampleValue** outputs;
    std::vector<std::string> paramNames;
    std::map<std::string, float> paramMap;
    std::map<std::string, RNBO::ParameterInfo> paramInfoMap;
    std::function<void(std::string, float, float, float)> configureSlider;
    void configureUI(std::function<int(std::string[], int, JavaVM*)> cDecPsRef, JavaVM* jvm);
    JavaVM** m_jvm;
    std::function<int(std::string[], int, JavaVM*)> m_cDecPsRef;
};

#endif //RNBO_TEST_MSPLOADER_H
