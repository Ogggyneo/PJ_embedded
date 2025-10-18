#include "globals.h"

AsyncWebServer server_1(80);

String NAME_DEVICE;
String WIFI_SSID;
String WIFI_PASS;
String TOKEN;
String EMAIL;
String HTTP_SELECT;
String webhook;
String auth_token;
String device_id;
String WARNING_VALUE;
String compare_temp;
String compare_humi;
String compare_sound;
String compare_pressure;
String compare_light;
String compare_pm2p5;
String compare_pm10;
String GOOGLE_SHEET_SELECT;
String spreadsheet_id;
String project_id;
String client_email;
String private_key;

void loadInfoFromFile()
{
    File file = LittleFS.open("/info.dat", "r");
    if (!file)
    {
        Serial.println("Không thể mở file để đọc");
        return;
    }
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
    }
    else
    {
        NAME_DEVICE = strdup(doc["NAME_DEVICE"]);
        WIFI_SSID = strdup(doc["WIFI_SSID"]);
        WIFI_PASS = strdup(doc["WIFI_PASS"]);
        TOKEN = strdup(doc["TOKEN"]);
        EMAIL = strdup(doc["EMAIL"]);
        HTTP_SELECT = strdup(doc["HTTP_SELECT"]);
        WARNING_VALUE = strdup(doc["WARNING_VALUE"]);
        GOOGLE_SHEET_SELECT = strdup(doc["GOOGLE_SHEET_SELECT"]);
        if (HTTP_SELECT == "yes")
        {
            webhook = strdup(doc["webhook"]);
            auth_token = strdup(doc["auth_token"]);
            device_id = strdup(doc["device_id"]);
        }
        if (WARNING_VALUE == "yes")
        {
            if (doc.containsKey("compare_temp"))
            {
                compare_temp = strdup(doc["compare_temp"]);
            }

            if (doc.containsKey("compare_humi"))
            {
                compare_humi = strdup(doc["compare_humi"]);
            }

            if (doc.containsKey("compare_sound"))
            {
                compare_sound = strdup(doc["compare_sound"]);
            }

            if (doc.containsKey("compare_pressure"))
            {
                compare_pressure = strdup(doc["compare_pressure"]);
            }

            if (doc.containsKey("compare_light"))
            {
                compare_light = strdup(doc["compare_light"]);
            }

            if (doc.containsKey("compare_pm2p5"))
            {
                compare_pm2p5 = strdup(doc["compare_pm2p5"]);
            }

            if (doc.containsKey("compare_pm10"))
            {
                compare_pm10 = strdup(doc["compare_pm10"]);
            }
        }
        if (GOOGLE_SHEET_SELECT == "yes")
        {
            spreadsheet_id = strdup(doc["spreadsheet_id"]);
            project_id = strdup(doc["project_id"]);
            client_email = strdup(doc["client_email"]);
            private_key = strdup(doc["private_key"]);
        }
    }
    file.close();
}

void deleteInfoFile()
{
    if (LittleFS.exists("/info.dat"))
    {
        LittleFS.remove("/info.dat");
    }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AIR QUALITY Configuration</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f2f2f2;
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }

        .container {
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            width: 300px;
        }

        h2 {
            text-align: center;
            color: #333;
        }

        label {
            margin-top: 10px;
            display: block;
            color: #666;
        }

        input[type="text"],
        input[type="password"],
        input[type="email"],
        select {
            width: 100%;
            padding: 10px;
            margin-top: 5px;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }

        input[type="submit"] {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 10px;
            border-radius: 4px;
            cursor: pointer;
            width: 100%;
            margin-top: 15px;
        }

        input[type="submit"]:hover {
            background-color: #45a049;
        }

        .ota-title {
            margin-top: 20px;
            font-size: 18px;
            text-align: center;
            color: #333;
        }

        .additional-inputs {
            display: none;
            margin-top: 10px;
        }

        hr {
            border: 0;
            border-top: 3px solid #333;
            margin: 20px 0;
        }

        select {
            text-align: left;
            /* Đặt text align cho toàn bộ select là trái */
        }

        select option {
            text-align: center;
            /* Căn giữa cho mỗi option trong select */
        }

        .label_main {
            font-weight: bold;
        }
    </style>
    <script>
        function toggleHTTPFields() {
            var httpSelect = document.getElementById("http_select");
            var additionalInputs = document.getElementById("additional_inputs");
            if (httpSelect.value === "yes") {
                additionalInputs.style.display = "block";
            } else {
                additionalInputs.style.display = "none";
            }
        }

        function toggleWarningFields() {
            var warningSelect = document.getElementById("warning_select");
            var warningInputs = document.getElementById("warning_inputs");
            if (warningSelect.value === "yes") {
                warningInputs.style.display = "block";
            } else {
                warningInputs.style.display = "none";
            }
        }

        function toggleGGSHEETFields() {
            var ggsheetSelect = document.getElementById("ggsheet_select");
            var additionalInputs = document.getElementById("ggsheet_inputs");
            if (ggsheetSelect.value === "yes") {
                additionalInputs.style.display = "block";
            } else {
                additionalInputs.style.display = "none";
            }
        }
    </script>
</head>

<body>
    <div class="container">
        <h2>ESP32 Configuration</h2>
        <form action="/save" method="post">
            <label for="name_device" class="label_main">Name Device :</label>
            <input type="text" name="name_device" id="name_device" required>

            <label for="ssid" class="label_main">WiFi SSID:</label>
            <input type="text" name="ssid" id="ssid" required>

            <label for="pass" class="label_main">WiFi Password:</label>
            <input type="password" name="pass" id="pass">

            <label for="token" class="label_main">MQTT Token:</label>
            <input type="text" name="token" id="token" required>

            <label for="email" class="label_main">Email:</label>
            <input type="email" name="email" id="email" required>

            <hr>

            <label for="http_select" class="label_main">HTTP :</label>
            <select name="http_select" id="http_select" onchange="toggleHTTPFields()" required>
                <option value="default">Default</option>
                <option value="yes">Yes</option>
            </select>


            <div id="additional_inputs" class="additional-inputs">
                <label for="webhook">Webhook Endpoint:</label>
                <input type="text" name="webhook" id="webhook">

                <label for="auth_token">Authorization Token:</label>
                <input type="text" name="auth_token" id="auth_token">

                <label for="device_id">ID Device:</label>
                <input type="text" name="device_id" id="device_id">
            </div>

            <hr>
            <label for="warning_select" class="label_main">Value warning :</label>
            <select name="warning_select" id="warning_select" onchange="toggleWarningFields()" required>
                <option value="default">Default</option>
                <option value="yes">Yes</option>
            </select>

            <div id="warning_inputs" class="additional-inputs">
                <!-- Temperature Section -->
                <label for="temperature">Temperature:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_temperature" id="comparison_operator_temperature"
                        style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="temperature_value" id="temperature_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- Humidity Section -->
                <label for="humidity">Humidity:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_humidity" id="comparison_operator_humidity" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="humidity_value" id="humidity_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- Sound Section -->
                <label for="sound">Sound:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_sound" id="comparison_operator_sound" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="sound_value" id="sound_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- Pressure Section -->
                <label for="pressure">Pressure:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_pressure" id="comparison_operator_pressure" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="pressure_value" id="pressure_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- Light Section -->
                <label for="light">Light:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_light" id="comparison_operator_light" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="light_value" id="light_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- PM2.5 Section -->
                <label for="pm2p5">PM2.5:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_pm2p5" id="comparison_operator_pm2p5" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="pm2p5_value" id="pm2p5_value" style="width: 48%;"
                        placeholder="Enter value">
                </div>

                <!-- PM10 Section -->
                <label for="pm10">PM10:</label>
                <div style="display: flex; justify-content: space-between;">
                    <select name="comparison_operator_pm10" id="comparison_operator_pm10" style="width: 48%;">
                        <option value="<">&lt;</option>
                        <option value=">">&gt;</option>
                        <option value="=">=</option>
                        <option value="<=">&lt;=</option>
                        <option value=">=">&gt;=</option>
                    </select>
                    <input type="text" name="pm10_value" id="pm10_value" style="width: 48%;" placeholder="Enter value">
                </div>
            </div>

            <hr />
            <label for="ggsheet_select" class="label_main">GOOGLE SHEET :</label>
            <select name="ggsheet_select" id="ggsheet_select" onchange="toggleGGSHEETFields()" required>
                <option value="default">Default</option>
                <option value="yes">Yes</option>
            </select>


            <div id="ggsheet_inputs" class="additional-inputs">
                <label for="spreadsheet_id">Spreadsheet ID :</label>
                <input type="text" name="spreadsheet_id" id="spreadsheet_id">

                <label for="project_id">PROJECT ID :</label>
                <input type="text" name="project_id" id="project_id">

                <label for="client_email">CLIENT EMAIL :</label>
                <input type="text" name="client_email" id="client_email">

                <label for="private_key">PRIVATE KEY :</label>
                <input type="text" name="private_key" id="private_key">
            </div>
            <input type="submit" value="Save">
        </form>
    </div>
</body>

</html>
)rawliteral";

void startAccessPoint()
{
#ifdef IS_ROOT
    LED_ACP();
#endif
    unsigned long previousMillis = 0;
    const long interval = 1000;
    int lastStationCount = -1;
    int textWidth = 0;
    int xPosition = 0;
    WiFi.softAP(SSID_AP);
    Serial.println("Access Point Started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    server_1.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send_P(200, "text/html", index_html); });

    server_1.on("/save", HTTP_POST, [](AsyncWebServerRequest *request)
                {
NAME_DEVICE = request->getParam("name_device", true)->value();
WIFI_SSID = request->getParam("ssid", true)->value();
WIFI_PASS = request->getParam("pass", true)->value();
TOKEN = request->getParam("token", true)->value();
EMAIL = request->getParam("email", true)->value();
HTTP_SELECT = request->getParam("http_select", true)->value();
WARNING_VALUE = request->getParam("warning_select", true)->value();
GOOGLE_SHEET_SELECT = request->getParam("ggsheet_select", true)->value();

if (HTTP_SELECT == "yes")
{
    webhook = request->getParam("webhook", true)->value();
    auth_token = request->getParam("auth_token", true)->value();
    device_id = request->getParam("device_id", true)->value();
}

if (WARNING_VALUE == "yes")
{
    if (request->hasParam("comparison_operator_temperature", true) && request->hasParam("temperature_value", true)) {
        String comparison_operator_temp = request->getParam("comparison_operator_temperature", true)->value();
        String temperature_value = request->getParam("temperature_value", true)->value();
        compare_temp = comparison_operator_temp + temperature_value;
    }
    
    if (request->hasParam("comparison_operator_humidity", true) && request->hasParam("humidity_value", true)) {
        String comparison_operator_humi = request->getParam("comparison_operator_humidity", true)->value();
        String humidity_value = request->getParam("humidity_value", true)->value();
        compare_humi = comparison_operator_humi + humidity_value;
    }
    
    if (request->hasParam("comparison_operator_sound", true) && request->hasParam("sound_value", true)) {
        String comparison_operator_sound = request->getParam("comparison_operator_sound", true)->value();
        String sound_value = request->getParam("sound_value", true)->value();
        compare_sound = comparison_operator_sound + sound_value;
    }
    
    if (request->hasParam("comparison_operator_pressure", true) && request->hasParam("pressure_value", true)) {
        String comparison_operator_pressure = request->getParam("comparison_operator_pressure", true)->value();
        String pressure_value = request->getParam("pressure_value", true)->value();
        compare_pressure = comparison_operator_pressure + pressure_value;
    }
    
    if (request->hasParam("comparison_operator_light", true) && request->hasParam("light_value", true)) {
        String comparison_operator_light = request->getParam("comparison_operator_light", true)->value();
        String light_value = request->getParam("light_value", true)->value();
        compare_light = comparison_operator_light + light_value;
    }
    
    if (request->hasParam("comparison_operator_pm2p5", true) && request->hasParam("pm2p5_value", true)) {
        String comparison_operator_pm2p5 = request->getParam("comparison_operator_pm2p5", true)->value();
        String pm2p5_value = request->getParam("pm2p5_value", true)->value();
        compare_pm2p5 = comparison_operator_pm2p5 + pm2p5_value;
    }
    
    if (request->hasParam("comparison_operator_pm10", true) && request->hasParam("pm10_value", true)) {
        String comparison_operator_pm10 = request->getParam("comparison_operator_pm10", true)->value();
        String pm10_value = request->getParam("pm10_value", true)->value();
        compare_pm10 = comparison_operator_pm10 + pm10_value;
    }
}

if (GOOGLE_SHEET_SELECT == "yes")
{
    spreadsheet_id =request->getParam("spreadsheet_id", true)->value();
    project_id = request->getParam("project_id", true)->value();
    client_email =request->getParam("client_email", true)->value();
    private_key = request->getParam("private_key", true)->value();
}



DynamicJsonDocument doc(4096);
doc["NAME_DEVICE"] = NAME_DEVICE;
doc["WIFI_SSID"] = WIFI_SSID;
doc["WIFI_PASS"] = WIFI_PASS;
doc["TOKEN"] = TOKEN;
doc["EMAIL"] = EMAIL;
doc["HTTP_SELECT"]=HTTP_SELECT;
doc["WARNING_VALUE"]=WARNING_VALUE;
doc["GOOGLE_SHEET_SELECT"]=GOOGLE_SHEET_SELECT;

if(HTTP_SELECT == "yes")
{
    doc["webhook"] = webhook;
    doc["auth_token"] = auth_token;
    doc["device_id"] = device_id;
}

if(WARNING_VALUE == "yes")
{
    if (!compare_temp.isEmpty()) {
        doc["compare_temp"] = compare_temp;
    }
    
    if (!compare_humi.isEmpty()) {
        doc["compare_humi"] = compare_humi;
    }
    
    if (!compare_sound.isEmpty()) {
        doc["compare_sound"] = compare_sound;
    }
    
    if (!compare_pressure.isEmpty()) {
        doc["compare_pressure"] = compare_pressure;
    }
    
    if (!compare_light.isEmpty()) {
        doc["compare_light"] = compare_light;
    }
    
    if (!compare_pm2p5.isEmpty()) {
        doc["compare_pm2p5"] = compare_pm2p5;
    }
    
    if (!compare_pm10.isEmpty()) {
        doc["compare_pm10"] = compare_pm10;
    }    
}

if (GOOGLE_SHEET_SELECT == "yes")
{
    doc["spreadsheet_id"] = spreadsheet_id;
    doc["project_id"] = project_id;
    doc["client_email"] = client_email;
    doc["private_key"] = private_key;
}

File configFile = LittleFS.open("/info.dat", "w");
if (configFile) {
serializeJson(doc, configFile);
configFile.close();
request->send(200, "text/html", "Configuration has been saved. ESP32 will restart...");
delay(1000);
ESP.restart();
} else {
request->send(500, "text/html", "Unable to save the configuration.");
} });

    server_1.begin();

#ifdef M5_CORE2
    while (true)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            int currentStationCount = WiFi.softAPgetStationNum();
            if (currentStationCount != lastStationCount)
            {
                lastStationCount = currentStationCount;

                lcd.clear(TFT_BLACK);
                lcd.waitDisplay();
                lcd.clear(TFT_WHITE);
                lcd.waitDisplay();

                if (currentStationCount == 0)
                {
                    String apQrcode = "WIFI:T:nopass;S:" + String(SSID_AP) + ";P:;H:false;;";
                    lcd.drawString("Scan me", (lcd.width() - lcd.textWidth("Scan me", &fonts::FreeSansBold9pt7b)) / 2, 15, &fonts::FreeSansBold9pt7b);
                    lcd.qrcode(apQrcode, 35, 35, 130);
                    textWidth = lcd.textWidth(String(SSID_AP), &fonts::FreeSansBold9pt7b);
                    xPosition = (lcd.width() - textWidth) / 2;
                    lcd.drawString(String(SSID_AP), xPosition, 175, &fonts::FreeSansBold9pt7b);
                }
                else
                {
                    String ipAddress = "http://" + WiFi.softAPIP().toString();
                    lcd.drawString("Scan me", (lcd.width() - lcd.textWidth("Scan me", &fonts::FreeSansBold9pt7b)) / 2, 15, &fonts::FreeSansBold9pt7b);
                    lcd.qrcode(ipAddress, 35, 35, 130);
                    textWidth = lcd.textWidth(String(ipAddress), &fonts::FreeSansBold9pt7b);
                    xPosition = (lcd.width() - textWidth) / 2;
                    lcd.drawString(String(ipAddress), xPosition, 175, &fonts::FreeSansBold9pt7b);
                }
                lcd.waitDisplay();
            }
        }
    }
#endif
}

void TaskResetDevice(void *pvParameters)
{
    unsigned long buttonPressStartTime = 0;
    while (true)
    {
        if (digitalRead(BOOT) == LOW)
        {
            if (buttonPressStartTime == 0)
            {
                buttonPressStartTime = millis();
            }
            else if (millis() - buttonPressStartTime > 5000)
            {
                deleteInfoFile();
                ESP.restart();
                vTaskDelete(NULL);
            }
        }
#ifdef M5_CORE2
        else if (M5.BtnB.wasPressed())
        {
            if (buttonPressStartTime == 0)
            {
                buttonPressStartTime = millis();
            }
            else if (millis() - buttonPressStartTime > 5000)
            {
                deleteInfoFile();
                ESP.restart();
                vTaskDelete(NULL);
            }
        }
#endif
        else
        {
            buttonPressStartTime = 0;
        }
        vTaskDelay(delay_connect / portTICK_PERIOD_MS);
    }
}

void reset_device()
{
    xTaskCreate(TaskResetDevice, "TaskResetDevice", 4096, NULL, 1, NULL);
}

bool check_info(bool check)
{

#ifdef IS_LEAF
    pinMode(BOOT, INPUT);
#endif
    if (!check)
    {
        loadInfoFromFile();
        reset_device();
    }
    if (NAME_DEVICE.isEmpty() || WIFI_SSID.isEmpty() || TOKEN.isEmpty())
    {
        if (!check)
        {
            startAccessPoint();
        }

        // if (HTTP_SELECT == "yes" && (webhook.isEmpty() || auth_token.isEmpty() || device_id.isEmpty()))
        // {
        //     startAccessPoint();
        // }
        return false;
    }
    return true;
}
