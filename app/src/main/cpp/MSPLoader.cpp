//
// Created by stuar on 23/01/2023.
//
static const char *TAG = "MSPLoader";

#include "android/log.h"
#include "MSPLoader.h"
//#include <stdlib.h>
#include "rnbo/src/platforms/stdlib/RNBO_PlatformInterfaceStdLib.h"

oboe::Result MSPLoader::open() {
    mErrorCallback = std::make_shared<MyErrorCallback>(this);

    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setFormat(oboe::AudioFormat::Float)
            ->setChannelCount(kChannelCount)
            ->setDataCallback(this)
            ->setErrorCallback(mErrorCallback)
                    // Open using a shared_ptr.
            ->openStream(mStream);

    if (result != oboe::Result::OK) {
        return result;
    }

    return oboe::Result::OK;
}

oboe::Result MSPLoader::start() {
    return mStream->requestStart();
}

oboe::Result MSPLoader::stop() {
    oboe::Result result = mStream->requestStop();
    return result;
}

oboe::Result MSPLoader::close() {
    return mStream->close();
}

void MSPLoader::updateParam(const char *paramName, float val) {
    int parameterIndex = rnboObject.getParameterIndexForID(paramName);
    RNBO::ParameterInfo pInfo = paramInfoMap[paramName];
    rnboObject.setParameterValue(parameterIndex, (val / 1000.0) * (pInfo.max - pInfo.min) + pInfo.min, rnboObject.getCurrentTime());
}

int MSPLoader::init(std::function<int(std::string[], int, JavaVM *)> cDecPsRef, JavaVM *jvm) {
    // Initialise RNBO
    // Set platform interface as stdlib.
    // RNBO::PlatformInterfaceStdLib();

    const int bPF = mStream->getBytesPerFrame();
    const int bPS = mStream->getBytesPerSample();

    // Get channel count of sketch and
    // display a warning if attributes
    // don't match.
    const int channelsSet = bPF / bPS;

    if (channelsSet != kChannelCount) {
        // TODO create warning popup
        __android_log_print(ANDROID_LOG_WARN, TAG,
                            "Warning: MSP Patch requires %i channels whilst %i are configured!",
                            channelsSet, kChannelCount);
    }

    // Sample frequency
    const int SF = mStream->getSampleRate();

    bool rnboResult = rnboObject.prepareToProcess(SF, 1024);

    // configureUI not yet implemented.
    configureUI(cDecPsRef, jvm);

    outputs = new RNBO::SampleValue *[kChannelCount];
    outputs[0] = new double[1024];

    return rnboResult;
}


void
MSPLoader::configureUI(std::function<int(std::string[], int, JavaVM *)> cDecPsRef, JavaVM *jvm) {
    // TODO Add UI configuration process
    // Initialise param map.
    paramMap = std::map<std::string, float>();

    int n = rnboObject.getNumParameters();

    std::string delimitedNames = "";


    for (int i = 0; i < n; i++) {
        // Self-explanatory
        const char *id = rnboObject.getParameterId(i);

        // TODO change this to std::string
        const char *name = rnboObject.getParameterName(i);
        __android_log_print(ANDROID_LOG_INFO, TAG,
                            "Parameter: %s", name);
        paramNames.emplace_back(std::string(name));

        // Set initial parameter values;
        float currentValue = rnboObject.getParameterValue(i);

        paramMap.emplace(name, currentValue);
        delimitedNames += std::string(name) + ",";

        RNBO::ParameterInfo tempInfo;
        rnboObject.getParameterInfo(i, &tempInfo);

        paramInfoMap.emplace(name, tempInfo);

        // This if statement filters out subpatcher parameters,
        // only printing the name of top level parameters. This
        // isn't necessary, so comment out this if statement if
        // you want to print out all parameters.
        if (strstr(id, "/") == nullptr) {
            __android_log_print(ANDROID_LOG_INFO, TAG,
                                "Parameter: %s", name);
        }
    }


    delimitedNames.resize(delimitedNames.size() - 1);
    cDecPsRef(paramNames.data(), n, jvm);

}

void MSPLoader::MyErrorCallback::onErrorAfterClose(oboe::AudioStream *oboeStream,
                                                   oboe::Result error) {
    __android_log_print(ANDROID_LOG_INFO, TAG,
                        "%s() - error = %s",
                        __func__,
                        oboe::convertToText(error)
    );
    // Try to open and start a new stream after a disconnect.
    if (mParent->open() == oboe::Result::OK) {
        mParent->start();
    }
}

oboe::DataCallbackResult
MSPLoader::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    rnboObject.process(nullptr, 0, outputs, 1, numFrames);
    float *fOutput = (float *) audioData;

    float temp = 0;

    for (unsigned int i = 0; i < numFrames; i++) {

        *fOutput++ = (float) (outputs[0][i]);
    }

    return oboe::DataCallbackResult::Continue;
}