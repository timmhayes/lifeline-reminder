#include "web_dashboard.h"

#include <WebServer.h>

#include "config.h"
#include "event_log.h"

namespace {
WebServer g_server(WEB_SERVER_PORT);

void HandleRoot(void) {
    String html;
    html += "<!DOCTYPE html>";
    html += "<html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Safety Check-In</title>";
    html += "<style>body { font-family: Arial, sans-serif; margin: 20px; } h2 { color: #333; } </style>";
    html += "</head><body>";
    html += "<h2>Safety Check-In Dashboard</h2>";
    html += "<p><a href='/log'>View Success Log</a></p>";
    html += "</body></html>";
    g_server.send(200, "text/html", html);
}

void HandleLog(void) {
    const String content = EventLog_ReadAll();
    g_server.send(200, "text/plain", content);
}
}  // namespace

void WebDashboard_Init(void) {
    g_server.on("/", HTTP_GET, HandleRoot);
    g_server.on("/log", HTTP_GET, HandleLog);
    g_server.begin();
}

void WebDashboard_Service(void) {
    g_server.handleClient();
}
