#include "event_log.h"

#include <LittleFS.h>

#include "config.h"

bool EventLog_Init(void) {
    if (!LittleFS.begin(true)) {
        return false;
    }

    if (!LittleFS.exists(SUCCESS_LOG_PATH)) {
        File file = LittleFS.open(SUCCESS_LOG_PATH, FILE_WRITE);
        if (!file) {
            return false;
        }
        file.close();
    }

    return true;
}

bool EventLog_AppendSuccess(const tm& local_time) {
    File file = LittleFS.open(SUCCESS_LOG_PATH, FILE_APPEND);
    if (!file) {
        return false;
    }

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local_time);
    file.printf("%s SUCCESS\n", timestamp);
    file.close();
    return true;
}

bool EventLog_Clear(void) {
    File file = LittleFS.open(SUCCESS_LOG_PATH, FILE_WRITE);
    if (!file) {
        return false;
    }
    file.print("");  // Clear the file content
    file.close();
    return true;
}

String EventLog_ReadAll(void) {
    File file = LittleFS.open(SUCCESS_LOG_PATH, FILE_READ);
    if (!file) {
        return "Log file is unavailable.";
    }

    String content = file.readString();
    file.close();

    if (content.length() == 0) {
        return "No success events recorded yet.";
    }

    return content;
}
