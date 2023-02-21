#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

SoftwareSerial rp2040(3,1);

#ifndef STASSID
#define STASSID "Neural Net"
#define STAPSK "toshith@2619"
#define APSSID "Color"
#define APPSK "toshith@2619"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* ssidap = APSSID;
const char* passwordap = APPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

String color = "(0,0,0)";

String retpage(){
  return("<!Doctype html>\
<html>\
<head>\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <meta charset=\"utf-8\">\
    <title>Color</title>\
    <style>\
        #color{\
            display: none;\
        }\
        input{\
            margin-top: 10vh;\
            width: 50vw;\
            height: 3em;\
            border: none;\
        }\
        body{\
            color: rgb(211, 247, 249);\
            background-color: black;\
            text-align: center;\
        }\
        #bg{\
            z-index: -1;\
            position: absolute;\
            margin: 0;\
            padding: 0;\
            top: 0;\
            left:0;\
            width: 100vw;\
            height: 100vh;\
            overflow: hidden;\
        }\
        #bgimg{\
            top: 0;\
            left:0;\
            min-width: 100vw;\
            min-height: 100vh;\
            opacity: 80%;\
        }\
        h1{\
            width: 90vw;\
            margin-left: 5vw;\
            text-align: center;\
            background-color: rgba(0,0,0,0.1);\
        }\
        h2{\
        margin-top: 5vh;\
        }\
        #submit{\
            min-width: 30vw;\
            max-width: max-content;\
            font-size: 30px;\
            padding-top: 0.5em;\
            padding-bottom: 0.5em;\
            outline: none;\
        }\
        @media(orientation: portrait){\
            input{\
                width: 80vw;\
            }\
            #submit{\
                font-size: 1em;\
                min-width: 50vw;\
            }\
        }\
    </style>\
</head>\
\
<body>\
    <h1>Set Color</h1>\
    <div id=\"bg\">\
        <img src=\"https://github.com/toshithh/SmartDeskLight/blob/main/bgimg.png?raw=true\" id=\"bgimg\">\
    </div>\
\
    <input type=\"color\" id=\"color-selector\" value=\"#C800FF\" onchange=\"setColor()\">\
    <form action=\"\" method=\"post\" enctype=\"text/plain\">\
        <input name=\"color\" id=\"color\" value=\"(200,0,255)\">\
        <input type=\"submit\" value=\"Change\" id=\"submit\">\
    </form>\
    <h2>"+color+"</h2>\
    <script>\
        function setColor(){\
            y = document.getElementById(\"color-selector\").value;\
            r = parseInt(y.slice(1,3), 16);\
            g = parseInt(y.slice(3,5), 16);\
            b = parseInt(y.slice(5,7), 16);\
            document.getElementById(\"color\").value = \"(\"+r+\",\"+g+\",\"+b+\")\";\
        }\
    </script>\
</body>\
</html>");
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", retpage());
  digitalWrite(led, 0);
}

void handlePlain() {
  if (server.method() != HTTP_POST) {
    handleRoot();
  } else {
    digitalWrite(led, 1);
    String f = server.arg("plain");
    int x = f.length();
    color = server.arg("plain").substring(6, x);
    f = f.substring(6, x);
    char msg[15];
    int i = 0;
    f.toCharArray(msg, f.length()+1);
    while (i<f.length()){
      rp2040.write(msg[i]);
      i++;
    }
    
    server.send(200, "text/html", retpage());
    digitalWrite(led, 0);
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  rp2040.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.softAP(ssidap, passwordap);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  /////////////////////////////////////
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  ////////////////////////////////////

  server.on("/", handlePlain);

  server.onNotFound(handleNotFound);

  server.begin();
}

void loop(void) {
  server.handleClient();
  ArduinoOTA.handle();
}
