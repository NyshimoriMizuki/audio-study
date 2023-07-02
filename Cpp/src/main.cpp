#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

#define max(a, b) ((a) > (b) ? (a) : (b))

static void checkError(PaError err)
{
    if (err != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

static int patestCallback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    float *in = (float *)input;
    (void)output;

    int displaySize = 100;
    printf("\r");

    float vol_l = 0, vol_r = 0;
    for (unsigned int i = 0; i < frameCount * 2; i++)
    {
        vol_l = max(vol_l, abs(in[i]));
        vol_r = max(vol_l, abs(in[i + 1]));
    }
    for (int i = 0; i < displaySize; i++)
    {
        float proportion = i / (float)displaySize;

        if (proportion <= vol_l || proportion <= vol_r)
            printf("#");
        else
            printf(" ");
    }

    fflush(stdout);

    return 0;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    PaError err = Pa_Initialize();
    checkError(err);

    int numDeviced = Pa_GetDeviceCount();
    printf("Number devices: %d\n", numDeviced);
    if (numDeviced < 0)
    {
        printf("Error on getting device count.\n");
    }
    else if (numDeviced == 0)
    {
        printf("No devices available for this machine.\n");
    }

    const PaDeviceInfo *deviceinfo;
    for (int i = 0; i < numDeviced; i++)
    {
        deviceinfo = Pa_GetDeviceInfo(i);
        printf("Device %d\n", i);
        printf("    Name: %s\n", deviceinfo->name);
        printf("    Max input channels: %d\n", deviceinfo->maxInputChannels);
        printf("    Max output channels: %d\n", deviceinfo->maxOutputChannels);
        printf("    Sample rate: %f\n", deviceinfo->defaultSampleRate);
    }

    PaStreamParameters inputParameters;
    memset(&inputParameters, 0, sizeof(PaStreamParameters));

    inputParameters.device = 1;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream *stream;

    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        patestCallback,
        NULL);
    checkError(err);

    checkError(Pa_StartStream(stream));
    Pa_Sleep(30 * 1000);
    checkError(Pa_StopStream(stream));

    checkError(Pa_StartStream(stream));
    checkError(Pa_Terminate());

    return EXIT_SUCCESS;
}
