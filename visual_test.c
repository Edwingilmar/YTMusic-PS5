/* ScrewU2Music v0.4: visible PS5 notification test.
 * Based on the PS5 payload SDK notification example.
 * This is not yet a graphical music player.
 */
#include <stdio.h>
#include <stdbool.h>
#define SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM 0xFE
int sceNotificationSend(int userId, bool isLogged, const char *payload);
static const char message[] =
"{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\","
"\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\","
"\"toastOverwriteType\":\"No\",\"isImmediate\":true,"
"\"priority\":100,\"viewData\":{"
"\"message\":{\"body\":\"ScrewU2Music v0.4\"},"
"\"subMessage\":{\"body\":\"Visual test running - player UI not implemented yet\"},"
"\"actions\":[]},"
"\"platformViews\":{\"previewDisabled\":{\"viewData\":{"
"\"icon\":{\"type\":\"Predefined\",\"parameters\":{\"icon\":\"download\"}},"
"\"message\":{\"body\":\"ScrewU2Music v0.4 started\"}}}}},"
"\"createdDateTime\":\"2026-11-18T22:00:00.000Z\","
"\"localNotificationId\":\"588193128\"}";
int main(void) {
  int result = sceNotificationSend(SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM, true, message);
  printf("[ScrewU2Music] notification result: 0x%08X\n", (unsigned)result);
  return result == 0 ? 0 : 1;
}
