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
}

void onClientRequestRoot() {
    String resp;
    if (!app.user.started) {
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
    app.user.unit = APP_UNIT_CELSIUS;
    return celsius;
}

float getTemperatureFromFahrenheit(float fahrenheit) {
    // TODO calculate absolute temperature value from given fahrenheit value
    app.user.unit = APP_UNIT_FAHRENHEIT;
    return fahrenheit;
}

// app callbacks

bool appStart() {
    // TODO start system and return true, if any error occurred returns false
    app.user.started = 1;
    return true;
}

bool appStop() {
    // TODO stop system and return true, if any error occurred returns false
    app.user.started = 0;
    app.user.unit = APP_UNIT_UNSET;
    doTemperatureStop();
    return true;
}

bool appSetTemperature(float temperature) {
    // TODO set temperature and return true, if any error occurred returns false
    app.user.temperature = temperature;
    return true;
}

bool appColourChange() {
    // TODO change colour and return true, if any error occurred returns false
    doColourChange();
    return true;
}

// app loops

void appLoopColourPulse() {
    // TODO do it while changing colour
    doTemperatureControl();
}

void appLoopConnecting() {
    // TODO do it while esp re-connecting to wifi
    doTemperatureControl();
}

void appLoopConnected() {
    // TODO do it when wifi connection is established
    doTemperatureControl();
}

// TEMPERATURE (DS18B20)

DS18B20 ds(DS_PIN);

void doTemperatureControl() {
    float celsius = ds.getTempC();
    float fahrenheit = ds.getTempF();
    if (app.user.started && app.user.unit != APP_UNIT_UNSET) {
        float temperature = app.user.unit == APP_UNIT_CELSIUS ? celsius : fahrenheit;
        if (temperature < app.user.temperature) doTemperatureStart();
        else doTemperatureStop();
    }

    app.current.celsius = String(celsius);
    app.current.fahrenheit = String(fahrenheit);
}

void doTemperatureStart() {
    digitalWrite(TEMPERATURE_PIN, HIGH);
}

void doTemperatureStop() {
    digitalWrite(TEMPERATURE_PIN, LOW);
}

// COLOUR CHANGE

void doColourChange() {
    digitalWrite(COLOUR_PIN, HIGH);
    cb_delay(COLOUR_DELAY, appLoopColourPulse);
    digitalWrite(COLOUR_PIN, LOW);
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