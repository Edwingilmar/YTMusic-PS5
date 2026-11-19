/* ScrewU2Music native PS5 audio hardware probe.
 * Independent of the stable tile installer and Sony web browser.
 * Produces a short original 440 Hz triangle-wave test tone.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
static void trace(const char *fmt, ...) {
  char line[256]; va_list ap; va_start(ap,fmt);
  int n=vsnprintf(line,sizeof(line),fmt,ap); va_end(ap);
  if(n<=0)return;
  int fd=open("/data/ScrewU2Music-AudioProbe.log",O_WRONLY|O_CREAT|O_APPEND,0666);
  if(fd<0) fd=open("/user/home/ScrewU2Music-AudioProbe.log",O_WRONLY|O_CREAT|O_APPEND,0666);
  if(fd>=0){write(fd,line,(size_t)(n<255?n:255));close(fd);}
  printf("%s",line);
}


int sceUserServiceInitialize(void *);
int sceUserServiceGetForegroundUser(int *);
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
    trace("AudioProbe start\\n");
    trace("before sceAudioOutInit\\n");
    int err = sceAudioOutInit();
    trace("sceAudioOutInit returned %d\\n",err);
    if (err < 0) { trace("AudioOut init failed: %d\\n", err); return 1; }
    int userId = -1;
    err = sceUserServiceInitialize(NULL);
    trace("[ScrewU2Music] UserService init: %d\\n", err);
    if (err == 0) {
        err = sceUserServiceGetForegroundUser(&userId);
        trace("[ScrewU2Music] Foreground user: %d (status %d)\\n", userId, err);
    }
    if (userId < 0) { trace("No foreground user found\\n"); return 4; }
    trace("before sceAudioOutOpen\\n");
    int handle = sceAudioOutOpen(userId, 0, 0, AUDIO_FRAMES, SAMPLE_RATE, 0);
    trace("sceAudioOutOpen returned %d\\n",handle);
    if (handle < 0) { trace("AudioOut open failed: %d\\n", handle); return 2; }
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
        if (err < 0) { trace("AudioOut output failed: %d\\n", err); sceAudioOutClose(handle); return 3; }
    }
    sceAudioOutClose(handle);
    trace("[ScrewU2Music] Native audio probe complete\\n");
    return 0;
}
