#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DS18B20.h>
#include "EEPROM.h"


#include "config/app_config.h";
#include "common_css.h";
#include "common_js.h";
#include "common_html.h";
#include "index_html.h";
#include "panel_html.h";

// WIFI

typedef void (*cb_delay_callback_func_t)(void);

struct wifi_credentials_s {
    String ssid;
    String password;  
};

typedef struct wifi_credentials_s wifi_credentials_t;

void cb_delay(long ms, cb_delay_callback_func_t callback = nullptr) {
    ms += millis();
    while(millis() < ms) if (callback) callback(); 
}

wifi_credentials_t wifi_credentials;

void wifi_connect(cb_delay_callback_func_t callback = nullptr) {
    Serial.print("\n\nConnecting to ");
    Serial.println(wifi_credentials.ssid);
    WiFi.mode(WIFI_STA);
    int strl = 100;
    char ssid_buff[strl];
    char password_buff[strl];
    wifi_credentials.ssid.toCharArray(ssid_buff, strl);
    wifi_credentials.password.toCharArray(password_buff, strl);
    WiFi.begin(ssid_buff, password_buff);
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
    Serial.print("Server Respond sending: ");
    Serial.println(code);
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
    pinMode(WATER_FLOW_PIN, OUTPUT);
    pinMode(WIFI_SETUP_PIN, INPUT);
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

String doOxygenRead() {
    String ret("");
    while (OXIGEN_SERIAL.available()) {
        ret = OXIGEN_SERIAL.readStringUntil(13);
    }
    return ret;
}

// TEMPERATURE (DS18B20)

DS18B20 ds(DS_PIN);

void doSensorsCheck() {
    long sensorsCheckTime = millis() / SENSOR_CHECK_PERIOD;
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

void doTemperatureControl(float celsius, float fahrenheit) {
    if (app.user.started && app.user.unit != APP_UNIT_UNSET) {
        float temperature = app.user.unit == APP_UNIT_CELSIUS ? celsius : fahrenheit;
        if (temperature < app.user.temperature) doHeatingStart();
        else doHeatingStop();
    }
}

// heating

void doHeatingStart() {
    digitalWrite(HEATING_PIN, HEATING_ON);
}

void doHeatingStop() {
    digitalWrite(HEATING_PIN, HEATING_OFF);
}

// COLOUR CHANGE

void doColourChange() {
    digitalWrite(COLOUR_PIN, COLOUR_ON);
    cb_delay(COLOUR_DELAY, appLoopColourPulse);
    digitalWrite(COLOUR_PIN, COLOUR_OFF);
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
    digitalWrite(WATER_FILL_PIN, WATER_FILL_ON);
}

void doWaterFillStop() {
    digitalWrite(WATER_FILL_PIN, WATER_FILL_OFF);
}

// water level sensor

void doWaterCheck() {
    if (!digitalRead(WATER_SENSOR_PIN)) {
        doWaterFillStop();
    }
}

// water flow out

void doWaterFlowClose() {
    digitalWrite(WATER_FLOW_PIN, WATER_FLOW_ON);
}

void doWaterFlowOpen() {
    digitalWrite(WATER_FLOW_PIN, WATER_FLOW_OFF);
}

// EEPROM (for wifi ssid/password)

void eeprom_init(long size = 1000) {
    if (!EEPROM.begin(size)) {
        Serial.println("EEPROM init failure.");
        ESP.restart();
    }
}

void wifi_credentials_load(long addr = 0) {
    wifi_credentials.ssid = EEPROM.readString(addr);
    addr += wifi_credentials.ssid.length() + 1;
    wifi_credentials.password = EEPROM.readString(addr);
}

void wifi_credentials_store(long addr, String ssid, String password) {
    EEPROM.writeString(addr, ssid);
    addr += ssid.length() + 1;
    EEPROM.writeString(addr, password);
    EEPROM.commit();
}

void wifi_setup() {
    const long idelay = 300;
    const long wifiaddr = 0;
    if (digitalRead(WIFI_SETUP_PIN) == WIFI_SETUP_ON) {
        Serial.println("Type WiFi SSID:");
        while (!Serial.available()) delay(idelay);
        String ssid = Serial.readString();
        ssid.trim();
        Serial.println("Type WiFi Password:");
        while (!Serial.available()) delay(idelay);
        String password = Serial.readString();
        password.trim();
        wifi_credentials_store(wifiaddr, ssid, password);
        Serial.println("Credentials are saved, press reset to restart..");
        while(true);
    }
    wifi_credentials_load(wifiaddr);
}

// SKETCH

void setup()
{
    app_init();
    Serial.begin(SERIAL_BAUDRATE);
    eeprom_init();
    wifi_setup();
    OXIGEN_SERIAL.begin(OXIGEN_SERIAL_BAUDRATE);
    ds.selectNext();
    wifi_connect(nullptr);
    server_init();
}

void loop()
{
    wifi_stablish(appLoopConnecting);
    appLoopConnected();
    server.handleClient();
}