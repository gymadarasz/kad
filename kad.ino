#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DS18B20.h>


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

void serverSend(int code, const char* content_type, const String& content) {
    // Serial.print("Server Respond sending: ");
    // Serial.print(code);
    // Serial.print(" - (");
    // Serial.print(content_type);
    // Serial.println(")");
    // Serial.println(content);
    server.send(code, content_type, content);
}

void server_init() {
    server.on("/", onClientRequestRoot);
    server.on("/start", onClientRequestStart);
    server.on("/stop", onClientRequestStop);
    server.on("/get-data", onClientRequestGetData);
    server.on("/set-celsius", onClientRequestSetCelsius);
    server.on("/set-fahrenheit", onClientRequestSetFahrenheit);
    server.on("/change-colour", onClientRequestColourChange);
    server.on("/set-timer", onClientRequestSetTimer);
    server.onNotFound([]() {
        serverSend(404, "text/plain", "Not Found\n\n");
    });
    server.begin();
}

#define APP_UNIT_UNSET 0
#define APP_UNIT_CELSIUS 1
#define APP_UNIT_FAHRENHEIT 2

struct app_current_data_s {
    String oxygen;
    String celsius;
    String fahrenheit;
    String remaining;
};

struct app_user_data_s {
    bool started;
    int unit;
    float temperature;
};

struct app_s {
    app_current_data_s current;
    app_user_data_s user;
    long timerEnd;
    long lastSensorsCheck;
};

typedef struct app_s app_t;

app_t app;

void app_init() {
    app.current.oxygen = "na.";
    app.current.celsius = "na.";
    app.current.fahrenheit = "na.";
    app.current.remaining = "na.";
    app.user.started = false;
    app.user.unit = APP_UNIT_UNSET;
    app.user.temperature = -1;
    app.timerEnd = 0;
    app.lastSensorsCheck = 0;
    
    pinMode(COLOUR_PIN, OUTPUT);
    pinMode(HEATING_PIN, OUTPUT);
    pinMode(WATER_FILL_PIN, OUTPUT);
}

void onClientRequestRoot() {
    Serial.println("Requested call: onClientRequestRoot");
    String resp;
    if (!app.user.started) {
        resp = index_html;
    } else {
        resp = panel_html;
    }
    resp.replace("{{ common_css }}", common_css);
    resp.replace("{{ common_js }}", common_js);
    resp.replace("{{ common_html }}", common_html);
    resp.replace("{{ APP_DATA_REFRESH_PERIOD }}", String(APP_DATA_REFRESH_PERIOD));
    resp.replace("{{ APP_CELSIUS_MIN }}", String(APP_CELSIUS_MIN));
    resp.replace("{{ APP_CELSIUS_MAX }}", String(APP_CELSIUS_MAX));
    resp.replace("{{ APP_FAHRENHEIT_MIN }}", String(APP_FAHRENHEIT_MIN));
    resp.replace("{{ APP_FAHRENHEIT_MAX }}", String(APP_FAHRENHEIT_MAX));
    serverSend(200, "text/html", resp);
}
void onClientRequestStart() {
    Serial.println("Requested call: onClientRequestStart");
    if (!appStart()) serverSend(400, "text/plain", "Start failed\n\n");
    else serverSend(200, "text/plain", "OK\n\n");
}

void onClientRequestStop() {
    Serial.println("Requested call: onClientRequestStop");
    if (!appStop()) serverSend(400, "text/plain", "Stop failed\n\n");
    else serverSend(200, "text/plain", "OK\n\n");
}

void onClientRequestGetData() {
    Serial.println("Requested call: onClientRequestGetData");
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
    serverSend(200, "text/json", resp);
}

void onClientRequestSetCelsius() {
    Serial.println("Requested call: onClientRequestSetCelsius");
    if (!server.hasArg("celsius")) serverSend(400, "text/plain", "Celsius argument is missing\n\n");
    else {
        int temperature = getTemperatureFromCelsius(server.arg("celsius").toFloat());
        if (!appSetTemperature(temperature)) serverSend(400, "text/plain", "Set temperature failed\n\n");
        else serverSend(200, "text/plain", "OK\n\n");
    }
}

void onClientRequestSetFahrenheit() {
    Serial.println("Requested call: onClientRequestSetFahrenheit");
    if (!server.hasArg("fahrenheit")) serverSend(400, "text/plain", "Fahrenheit argument is missing\n\n");
    else {
        int temperature = getTemperatureFromFahrenheit(server.arg("fahrenheit").toFloat());
        if (!appSetTemperature(temperature)) serverSend(400, "text/plain", "Set temperature failed\n\n");
        else serverSend(200, "text/plain", "OK\n\n");
    }
}
 
void onClientRequestColourChange() {
    Serial.println("Requested call: onClientRequestColourChange");
    if (!appColourChange()) serverSend(400, "text/plain", "Colour change failed\n\n");
    else serverSend(200, "text/plain", "OK\n\n");
}

void onClientRequestSetTimer() {
    if (!server.hasArg("timer")) server.send(400, "text/plain", "Timer argument is missing\n\n");
    else {
        app.timerEnd = millis() + server.arg("timer").toInt() * 60 * 1000;
        server.send(200, "text/plain", "OK\n\n");
    }
}

// helpers

float getTemperatureFromCelsius(float celsius) {
    app.user.unit = APP_UNIT_CELSIUS;
    return celsius;
}

float getTemperatureFromFahrenheit(float fahrenheit) {
    app.user.unit = APP_UNIT_FAHRENHEIT;
    return fahrenheit;
}

// app callbacks

bool appStart() {
    app.user.started = 1;
    doWaterFlowClose();
    doWaterFillStart();
    doTimerStart();
    return true;
}

bool appStop() {
    app.user.started = 0;
    app.user.unit = APP_UNIT_UNSET;
    doHeatingStop();
    doWaterFillStop();
    doWaterFlowOpen();
    return true;
}

bool appSetTemperature(float temperature) {
    app.user.temperature = temperature;
    return true;
}

bool appColourChange() {
    doColourChange();
    return true;
}

// app loops

void appLoopColourPulse() {
    appLoopAll();
}

void appLoopConnecting() {
    appLoopAll();
}

void appLoopConnected() {
    appLoopAll();
}

void appLoopAll() {
    doSensorsCheck();
    doTimerCheck();
    doWaterCheck();
}

// OXYGEN SENSOR

SoftwareSerial oxygenSerial(OXYGEN_RX_PIN, OXYGEN_TX_PIN);

String doOxygenRead() {
    if (oxygenSerial.available()) {
        return oxygenSerial.readStringUntil('\r');
    }
    return String("");
}

// TEMPERATURE (DS18B20)

DS18B20 ds(DS_PIN);

void doSensorsCheck() {
    long sensorsCheckTime = millis() / DS_CHECK_PERIOD;
    if (sensorsCheckTime != app.lastSensorsCheck) {
        app.lastSensorsCheck = sensorsCheckTime;

        float celsius = ds.getTempC();
        float fahrenheit = ds.getTempF();
        
        doTemperatureControl(celsius, fahrenheit);

        app.current.celsius = String(celsius);
        app.current.fahrenheit = String(fahrenheit);

        String oxygen = doOxygenRead();
        if (oxygen != "") app.current.oxygen = oxygen;
    }
}

void doTemperatureControl() {
    if (app.user.started && app.user.unit != APP_UNIT_UNSET) {
        float temperature = app.user.unit == APP_UNIT_CELSIUS ? celsius : fahrenheit;
        if (temperature < app.user.temperature) doHeatingStart();
        else doHeatingStop();
    }
}

// heating

void doHeatingStart() {
    digitalWrite(HEATING_PIN, HIGH);
}

void doHeatingStop() {
    digitalWrite(HEATING_PIN, LOW);
}

// COLOUR CHANGE

void doColourChange() {
    digitalWrite(COLOUR_PIN, HIGH);
    cb_delay(COLOUR_DELAY, appLoopColourPulse);
    digitalWrite(COLOUR_PIN, LOW);
}

// timer

void doTimerStart() {
    app.timerEnd = millis() + WATER_TIMER;
}

void doTimerCheck() {
    int mins = 0;
    int secs = 0;
    long lefts = app.timerEnd - millis();
    if (lefts <= 0) {
        app.timerEnd = millis(); // block timer over turn
        doWaterFillStop();
        doHeatingStop();
    } else {
        mins = lefts / (60 * 1000);
        secs = lefts % (60 * 1000) / 1000;;
    }

    size_t size = 10;
    char buff[size] = {0};
    snprintf(buff, size, "%02d:%02d", mins, secs);
    app.current.remaining = buff;
}

// water infill

void doWaterFillStart() {
    digitalWrite(WATER_FILL_PIN, HIGH);
}

void doWaterFillStop() {
    digitalWrite(WATER_FILL_PIN, LOW);
}

// water level sensor

void doWaterCheck() {
    if (!digitalRead(WATER_SENSOR_PIN)) {
        doWaterFillStop();
    }
}

// water flow out

void doWaterFlowClose() {
    digitalWrite(WATER_FLOW_PIN, HIGH);
}

void doWaterFlowOpen() {
    digitalWrite(WATER_FLOW_PIN, LOW);
}


// SKETCH

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);
    ds.selectNext();
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