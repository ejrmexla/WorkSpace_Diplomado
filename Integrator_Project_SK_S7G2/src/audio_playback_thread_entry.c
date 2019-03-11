#include "audio_playback_thread.h"

void audio_playback_thread_entry(void);

/* Audio Playback entry function */
void audio_playback_thread_entry(void)
{
    /* TODO: add your own code here */
    while (1)
    {
        tx_thread_sleep (1);
    }
}
