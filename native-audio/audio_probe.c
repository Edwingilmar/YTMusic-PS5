/* ScrewU2Music native PS5 audio hardware probe.
 * Independent of the stable tile installer and Sony web browser.
 * Produces a short original 440 Hz triangle-wave test tone.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int sceAudioOutInit(void);
int sceAudioOutOpen(int userId, int type, int index, uint32_t len, uint32_t freq, uint32_t param);
int sceAudioOutOutput(int handle, const void *ptr);
int sceAudioOutClose(int handle);

/* SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO = 0 on PS4/PS5 SDKs.
 * Probe result must be validated on target firmware before integration.
 */
#define AUDIO_FRAMES 256
#define SAMPLE_RATE 48000
#define TEST_SECONDS 3

int main(void) {
    printf("[ScrewU2Music] Native audio hardware probe starting\\n");
    int err = sceAudioOutInit();
    if (err < 0) { printf("AudioOut init failed: %d\\n", err); return 1; }
    int handle = sceAudioOutOpen(0, 0, 0, AUDIO_FRAMES, SAMPLE_RATE, 0);
    if (handle < 0) { printf("AudioOut open failed: %d\\n", handle); return 2; }
    int16_t pcm[AUDIO_FRAMES * 2];
    uint32_t phase = 0;
    /* Triangle wave generated entirely locally; no streamed or copyrighted media. */
    for (int block = 0; block < (SAMPLE_RATE * TEST_SECONDS / AUDIO_FRAMES); ++block) {
        for (int i = 0; i < AUDIO_FRAMES; ++i) {
            uint32_t pos = phase % SAMPLE_RATE;
            int32_t wave = pos < SAMPLE_RATE / 2
                ? (int32_t)(pos * 4 * 3000 / SAMPLE_RATE) - 3000
                : 9000 - (int32_t)(pos * 4 * 3000 / SAMPLE_RATE);
            pcm[i * 2] = pcm[i * 2 + 1] = (int16_t)wave;
            phase += 440;
        }
        err = sceAudioOutOutput(handle, pcm);
        if (err < 0) { printf("AudioOut output failed: %d\\n", err); sceAudioOutClose(handle); return 3; }
    }
    sceAudioOutClose(handle);
    printf("[ScrewU2Music] Native audio probe complete\\n");
    return 0;
}
