#include "audio.hpp"

void AudioController::playWave(const uint8_t waveId, const uint16_t pitch, const uint16_t time, const uint8_t volume)
{
    // Calculate how many samples a wave of `pitch` frequency should be
    const float kDesiredLambda = 48000.0 / ((float) pitch);

    SoundChannel_t newChannelInfo = { 0.0, 0.0, 0, 0, 0 };

    newChannelInfo.channelWave = waveId;
    newChannelInfo.jump = 600 / kDesiredLambda;
    newChannelInfo.phase = 0;
    newChannelInfo.timeout = 48 * time;
    newChannelInfo.volume = volume;

    for (uint32_t i = 0; i < kChannelNumber; i++)
    {
        if (Channels[i].channelWave == 0 || Channels[i].volume == 0)
        {
            Channels[i] = newChannelInfo;
            break;
        }
    }
}

void AudioController::processAudioCallback(bool secondHalf)
{
    uint16_t offset = secondHalf ? 128 : 0;
    uint16_t end = 128 + offset;

    for (uint_fast16_t j = offset; j < end; j+=2)
    {
        float tempSampleAllChannels = 0;

        for (uint_fast8_t i = 0; i < kChannelNumber; i++)
        {
            float tempSamplePerChannel = 0;
            SoundChannel_t currentChannel = Channels[i];

            // skip if channel is muted
            if (currentChannel.volume == 0)
                continue;

            float currentRawPhase = currentChannel.phase;
            uint32_t currentPhase = static_cast<int>(currentRawPhase + 0.5) % 600;
            
            switch (currentChannel.channelWave)
            {
                case 1: tempSamplePerChannel = kSineWave[currentPhase];         break;
                case 2: tempSamplePerChannel = kAKWF_eguitar_002[currentPhase]; break;
                case 3: tempSamplePerChannel = kAKWF_ebass_061[currentPhase];   break;
                default: break;
            }

            tempSampleAllChannels += (tempSamplePerChannel * currentChannel.volume) / 128.0;

            float nextPhase = (currentRawPhase + currentChannel.jump);
            while (nextPhase > 600)
            {
                nextPhase -= 600;
            }

            Channels[i].phase = nextPhase;

            // reset channel if timeout == 0
            if (--Channels[i].timeout == 0)
            {
                Channels[i] = SoundChannel_t { 0.0, 0.0, 0, 0, 0 };
            }
        }

        if (tempSampleAllChannels > 32767)
            tempSampleAllChannels = 32767;

        if (tempSampleAllChannels < -32768)
            tempSampleAllChannels = -32768;

        int16_t clampedSample = static_cast<int>(tempSampleAllChannels);

        (*_audioDMABuffer)[j] = clampedSample;
        (*_audioDMABuffer)[j+1] = clampedSample;
    }
}