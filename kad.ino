#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>


#include "config/app_config.h";
#include "config/wifi_config.h";
#include "common_css.h";
#include "common_js.h";
#include "common_html.h";
#include "index_html.h";
#include "panel_html.h";

// WIFI

typedef void (*cb_delay_callback_func_t)(void);

void cb_delay(long ms, cb_delay_callback_func_t callback = nullptr) {
    ms += millis();
    while(millis() < ms) if (callback) callback(); 
}

char* wifi_get_ssid() {
    return WIFI_SSID; // TODO load from flash
}

char* wifi_get_password() {
    return WIFI_PASSWORD; // TODO load from flash
}

void wifi_connect(cb_delay_callback_func_t callback = nullptr) {
    Serial.print("\n\nConnecting to ");
    Serial.println(wifi_get_ssid());
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_get_ssid(), wifi_get_password());
    while (WiFi.status() != WL_CONNECTED)
    {
        cb_delay(500, callback);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
    Serial.println();
}

void wifi_stablish(cb_delay_callback_func_t callback = nullptr) {
    if (WiFi.status() != WL_CONNECTED) wifi_connect(callback);
}

// SERVER

WebServer server(SERVER_PORT);

void server_init() {
    server.on("/", onClientRequestRoot);
    server.on("/start", onClientRequestStart);
    server.on("/stop", onClientRequestStop);
    server.on("/get-data", onClientRequestGetData);
    server.on("/set-celsius", onClientRequestSetCelsius);
    server.on("/set-fahrenheit", onClientRequestSetFahrenheit);
    server.on("/change-colour", onClientRequestColourChange);
    server.onNotFound([]() {
        server.send(404, "text/plain", "Not Found\n\n");
    });
    server.begin();
}

struct app_current_data_s {
    String oxygen;
    String celsius;
    String fahrenheit;
    String remaining;
};

struct app_s {
    bool started;
    app_current_data_s current;
    long timerEnd;
};

typedef struct app_s app_t;

app_t app;

void app_init() {
    app.started = false;
    app.current.oxygen = "na.";
    app.current.celsius = "na.";
    app.current.fahrenheit = "na.";
    app.current.remaining = "na.";
    app.timerEnd = 0;
}

void onClientRequestRoot() {
    String resp;
    if (!app.started) {
        resp = index_html;
    } else {
        resp = panel_html;
    }
    resp.replace("{{ common_css }}", common_css);
    resp.replace("{{ common_js }}", common_js);
    resp.replace("{{ common_html }}", common_html);
    server.send(200, "text/html", resp);
}
void onClientRequestStart() {
    if (!appStart()) server.send(400, "text/plain", "Start failed\n\n");
    else server.send(200, "text/plain", "OK\n\n");
}

void onClientRequestStop() {
    if (!appStop()) server.send(400, "text/plain", "Stop failed\n\n");
    else server.send(200, "text/plain", "OK\n\n");
}

void onClientRequestGetData() {
    String resp(R"RESPONSE_JSON({
        "oxygen": "{{ oxygen }}",
        "celsius": "{{ celsius }}",
        "fahrenheit": "{{ fahrenheit }}",
        "remaining": "{{ remaining }}"
    })RESPONSE_JSON");
    resp.replace("{{ oxygen }}", app.current.oxygen);
    resp.replace("{{ celsius }}", app.current.celsius);
    resp.replace("{{ fahrenheit }}", app.current.fahrenheit);
    resp.replace("{{ remaining }}", app.current.remaining);
    server.send(200, "text/json", resp);
}

void onClientRequestSetCelsius() {
    if (!server.hasArg("celsius")) server.send(400, "text/plain", "Celsius argument is missing\n\n");
    else {
        int temperature = getTemperatureFromCelsius(server.arg("celsius").toFloat());
        if (!appSetTemperature(temperature)) server.send(400, "text/plain", "Set temperature failed\n\n");
        else server.send(200, "text/plain", "OK\n\n");
    }
}

void onClientRequestSetFahrenheit() {
    if (!server.hasArg("fahrenheit")) server.send(400, "text/plain", "Fahrenheit argument is missing\n\n");
    else {
        int temperature = getTemperatureFromFahrenheit(server.arg("fahrenheit").toFloat());
        if (!appSetTemperature(temperature)) server.send(400, "text/plain", "Set temperature failed\n\n");
        else server.send(200, "text/plain", "OK\n\n");
    }
}

void onClientRequestColourChange() {
    if (!appColourChange()) server.send(400, "text/plain", "Colour change failed\n\n");
    else server.send(200, "text/plain", "OK\n\n");
}

// helpers

float getTemperatureFromCelsius(float celsius) {
    // TODO calculate absolute temperature value from given celsius value
    return celsius;
}

float getTemperatureFromFahrenheit(float fahrenheit) {
    // TODO calculate absolute temperature value from given fahrenheit value
    return fahrenheit;
}

// app callbacks

bool appStart() {
    // TODO start system and return true, if any error occurred returns false
    app.started = 1;
    return true;
}

bool appStop() {
    // TODO stop system and return true, if any error occurred returns false
    app.started = 0;
    doTimerStart();
    return true;
}

bool appSetTemperature(float temperature) {
    // TODO set temperature and return true, if any error occurred returns false
    return true;
}

bool appColourChange() {
    // TODO change colour and return true, if any error occurred returns false
    return true;
}

// app loops

void appLoopConnecting() {
    // TODO do it while esp re-connecting to wifi
    doTimerCheck();
}

void appLoopConnected() {
    // TODO do it when wifi connection is established
    doTimerCheck();
}

// timer

void doTimerStart() {
    app.timerEnd = millis() + WATER_TIMER;
    doWaterStart();
}

void doTimerCheck() {
    int mins = 0;
    int secs = 0;
    long lefts = app.timerEnd - millis();
    if (lefts <= 0) {
        app.timerEnd = millis(); // block timer over turn
        doWaterStop();
    } else {
        int mins = lefts / (60 * 1000);
        int secs = lefts % (60 * 1000);
    }

    String remaining("");
    remaining += mins < 10 ? "0" : "";
    remaining += String(mins);
    remaining += ":";
    remaining += secs < 10 ? "0" : "";
    remaining += String(secs);
    app.current.remaining = remaining; 
}

// water flow

void doWaterStart() {
    digitalWrite(WATER_FLOW_PIN, HIGH);
}

void doWaterStop() {
    digitalWrite(WATER_FLOW_PIN, LOW);
}



// SKETCH

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);
    wifi_connect(nullptr);
    server_init();
    app_init();
}

void loop()
{
    wifi_stablish(appLoopConnecting);
    appLoopConnected();
    server.handleClient();
}