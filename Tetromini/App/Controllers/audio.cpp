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

    int16_t* begin = (*_audioDMABuffer).begin();

    // Empty out this half of the buffer
    std::fill(begin + offset, begin + end, 0);

    // Entirely skip unused channels
    for (uint_fast8_t c = 0; c < kChannelNumber; c++)
    {
        SoundChannel_t currentChannel = Channels[c];
        float currentChannelPhase = currentChannel.phase;
        float currentChannelJump = currentChannel.jump;
        uint32_t currentChannelTimeout = currentChannel.timeout;
        uint8_t currentChannelWave = currentChannel.channelWave;
        uint8_t currentChannelVolume = currentChannel.volume;

        // skip if channel is muted
        if (currentChannelVolume == 0)
            continue;

        // Take advantage of contiguous memory
        for (uint_fast16_t s = offset; s < end; s+=2)
        {
            // Retrieve sample affected by other channels and initalize int16 version of it
            float CurrentDMASampleAsFloat = (*_audioDMABuffer)[s];
            int16_t CurrentDMASample = 0;

            // Get current sample from channel and scale its volume
            float SampleToApply = 0.0f;
            uint32_t currentPhase = static_cast<uint32_t>(currentChannelPhase + 0.5) % 600;
            
            switch (currentChannelWave)
            {
                case 1: SampleToApply += kSineWave[currentPhase];         break;
                case 2: SampleToApply += kAKWF_eguitar_002[currentPhase]; break;
                case 3: SampleToApply += kAKWF_ebass_061[currentPhase];   break;
                default: break;
            }

            SampleToApply *= currentChannelVolume / 128.0f;
            CurrentDMASampleAsFloat += SampleToApply;

            // Proceed to next sample inside channel
            currentChannelPhase += currentChannelJump;
            while (currentChannelPhase > 600)
            {
                currentChannelPhase -= 600;
            }

            if (CurrentDMASampleAsFloat > 32767)
                CurrentDMASample = 32767;
            else if (CurrentDMASampleAsFloat < -32768)
                CurrentDMASample = -32768;
            else
                CurrentDMASample = static_cast<int16_t>(CurrentDMASampleAsFloat);

            (*_audioDMABuffer)[s] = CurrentDMASample;
            (*_audioDMABuffer)[s+1] = CurrentDMASample;

            // reset channel if timeout == 0
            if (--currentChannelTimeout == 0)
            {
                currentChannelJump = currentChannelPhase = 0.0;
                currentChannelVolume = currentChannelWave = 0;
                break;
            }
        }

        // Save altered channel info
        Channels[c] = SoundChannel_t {currentChannelPhase, currentChannelJump, currentChannelTimeout, currentChannelWave, currentChannelVolume};
    }
}