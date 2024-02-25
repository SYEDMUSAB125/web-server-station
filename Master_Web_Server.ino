#include <Arduino_JSON.h>

#include <AsyncEventSource.h>
// #include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h>



#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>







//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
// Include the ArduinoJson library
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <Arduino_JSON.h>
//---------------------------------------- 

#include "PageIndex.h" 

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 15
#define rst 16
#define dio0 2
//----------------------------------------

//---------------------------------------- Variable declaration for your network credentials.
 // Your WiFi password
//----------------------------------------

//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = ""; 
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
byte LocalAddress = 0x01;               //--> address of this device (Master Address).
byte Destination_ESP32_Slave_1 = 0x02;  //--> destination to send to Slave 1 (ESP32).
byte Destination_ESP32_Slave_2 = 0x03;  //--> destination to send to Slave 2 (ESP32).
const byte get_Data_Mode = 1;           //--> Mode to get the reading status of the DHT11 sensor, humidity value, temperature value, state of LED 1 and LED 2.
const byte led_Control_Mode = 2;        //--> Mode to control LED 1 and LED 2 on the targeted Slave.
//---------------------------------------- 

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_SendMSG_to_GetData = 0;
const long interval_SendMSG_to_GetData = 1000;

unsigned long previousMillis_RestartLORA = 0;
const long interval_RestartLORA = 1000;
//---------------------------------------- 

//---------------------------------------- Variables to accommodate the reading status of the DHT11 sensor, humidity value, temperature value, state of LED 1 and LED 2.
int Humd[2];
float Temp[2];
String LED_1_State_str = "";
String LED_2_State_str = "";
String receive_Status_Read_DHT11 = "";
bool LED_1_State_bool = false;
bool LED_2_State_bool = false;
//---------------------------------------- 

//---------------------------------------- The variables used to check the parameters passed in the URL.
// Look in the "PageIndex.h" file.
// xhr.open("GET", "set_LED?Slave_Num="+slave+"&LED_Num="+led_num+"&LED_Val="+value, true);
// For example :
// set_LED?Slave_Num=S1&LED_Num=1&LED_Val=1
// PARAM_INPUT_1 = S1
// PARAM_INPUT_2 = 1
// PARAM_INPUT_3 = 1
const char* PARAM_INPUT_1 = "Slave_Num";
const char* PARAM_INPUT_2 = "LED_Num";
const char* PARAM_INPUT_3 = "LED_Val";
//---------------------------------------- 

//---------------------------------------- Variable declaration to hold data from the web server to control the LED.
String Slave_Number = "";
String LED_Number = "";
String LED_Value = "";
//---------------------------------------- 

// Variable declaration to count slaves.
byte Slv = 0;

// Variable declaration to get the address of the slaves.
byte slave_Address;

// Declaration of variable as counter to restart Lora Ra-02.
byte count_to_Rst_LORA = 0;

// Variable declaration to notify that the process of receiving the message has finished.
bool finished_Receiving_Message = false;

// Variable declaration to notify that the process of sending the message has finished.
bool finished_Sending_Message = false;

// Variable declaration to start sending messages to Slaves to control the LEDs.
bool send_Control_LED = false;

// Initialize JSONVar
JSONVar JSON_All_Data_Received;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination, byte SendMode) { 
  finished_Sending_Message = false;

  Serial.println();
  Serial.println("Tr to  : 0x" + String(Destination, HEX));
  Serial.print("Mode   : ");
  if (SendMode == 1) Serial.println("Getting Data");
  if (SendMode == 2) Serial.println("Controlling LED.");
  Serial.println("Message: " + Outgoing);
  
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.write(SendMode);           //--> 
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
  
  finished_Sending_Message = true;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  finished_Receiving_Message = false;

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data / message.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  // Resets the value of the count_to_Rst_LORA variable if a message is received.
  count_to_Rst_LORA = 0;

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println();
    Serial.println("er"); //--> "er" = error: message length does not match length.
    //Serial.println("error: message length does not match length");
    finished_Receiving_Message = true;
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println();
    Serial.println("!");  //--> "!" = This message is not for me.
    //Serial.println("This message is not for me.");
    finished_Receiving_Message = true;
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //----------------------------------------  if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Rc from: 0x" + String(sender, HEX));
  Serial.println("Message: " + Incoming);
  //---------------------------------------- 

  // Get the address of the senders or slaves.
  slave_Address = sender;

  // Calls the Processing_incoming_data() subroutine.
  Processing_incoming_data();

  finished_Receiving_Message = true;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines to process data from incoming messages.
void Processing_incoming_data() {
  
//  Examples of the contents of messages received from slaves are as follows: "s,80,30.50,1,0" , 
//  to separate them based on the comma character, the "GetValue" subroutine is used and the order is as follows:
//  GetValue(Incoming, ',', 0) = s
//  GetValue(Incoming, ',', 1) = 80
//  GetValue(Incoming, ',', 2) = 30.50
//  GetValue(Incoming, ',', 3) = 1
//  GetValue(Incoming, ',', 4) = 0

  //---------------------------------------- Conditions for processing data or messages from Slave 1 (ESP32 Slave 1).
  if (slave_Address == Destination_ESP32_Slave_1) {
    receive_Status_Read_DHT11 = GetValue(Incoming, ',', 0);
    if (receive_Status_Read_DHT11 == "f") receive_Status_Read_DHT11 = "FAILED";
    if (receive_Status_Read_DHT11 == "s") receive_Status_Read_DHT11 = "SUCCEED";
    Humd[0] = GetValue(Incoming, ',', 1).toInt();
    Temp[0] = GetValue(Incoming, ',', 2).toFloat();
    LED_1_State_str = GetValue(Incoming, ',', 3);
    LED_2_State_str = GetValue(Incoming, ',', 4);
    if (LED_1_State_str == "1" || LED_1_State_str == "0") {
      LED_1_State_bool = LED_1_State_str.toInt();
    }
    if (LED_2_State_str == "1" || LED_2_State_str == "0") {
      LED_2_State_bool = LED_2_State_str.toInt();
    }

    // Calls the Send_Data_to_WS() subroutine.
    Send_Data_to_WS("S1", 1);
  }
  //---------------------------------------- 

  //---------------------------------------- Conditions for processing data or messages from Slave 2 (ESP32 Slave 2).
  if (slave_Address == Destination_ESP32_Slave_2) {
    receive_Status_Read_DHT11 = GetValue(Incoming, ',', 0);
    if (receive_Status_Read_DHT11 == "f") receive_Status_Read_DHT11 = "FAILED";
    if (receive_Status_Read_DHT11 == "s") receive_Status_Read_DHT11 = "SUCCEED";
    Humd[1] = GetValue(Incoming, ',', 1).toInt();
    Temp[1] = GetValue(Incoming, ',', 2).toFloat();
    LED_1_State_str = GetValue(Incoming, ',', 3);
    LED_2_State_str = GetValue(Incoming, ',', 4);
    if (LED_1_State_str == "1" || LED_1_State_str == "0") {
      LED_1_State_bool = LED_1_State_str.toInt();
    }
    if (LED_2_State_str == "1" || LED_2_State_str == "0") {
      LED_2_State_bool = LED_2_State_str.toInt();
    }

    // Calls the Send_Data_to_WS() subroutine.
    Send_Data_to_WS("S2", 2);
  }
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to send data received from Slaves to the web server to be displayed.
void Send_Data_to_WS(char ID_Slave[5], byte Slave) {
  //:::::::::::::::::: Enter the received data into JSONVar(JSON_All_Data_Received).
  JSON_All_Data_Received["ID_Slave"] = ID_Slave;
  JSON_All_Data_Received["StatusReadDHT11"] = receive_Status_Read_DHT11;
  JSON_All_Data_Received["Humd"] = Humd[Slave-1];
  JSON_All_Data_Received["Temp"] = Temp[Slave-1];
  JSON_All_Data_Received["LED1"] = LED_1_State_bool;
  JSON_All_Data_Received["LED2"] = LED_2_State_bool; 
  //:::::::::::::::::: 
  
  //:::::::::::::::::: Create a JSON String to hold all data received from the sender.
  String jsonString_Send_All_Data_received = JSON.stringify(JSON_All_Data_Received);
  //:::::::::::::::::: 
  
  //:::::::::::::::::: Sends all data received from the sender to the browser as an event ('allDataJSON').
  events.send(jsonString_Send_All_Data_received.c_str(), "allDataJSON", millis());
  //::::::::::::::::::  
}
//________________________________________________________________________________ 

//________________________________________________________________________________ String function to process the data received

String GetValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to reset Lora Ra-02.
void Rst_LORA() {
  LoRa.setPins(ss, rst, dio0);

  Serial.println();
  Serial.println("Restart LoRa...");
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");

  // Reset the value of the count_to_Rst_LORA variable.
  count_to_Rst_LORA = 0;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  const char* ssid = "MyHomeWiFi";        // Your WiFi SSID
const char* password = "MyPassword123";
  // put your setup code here, to run once:

  Serial.begin(115200);

  //---------------------------------------- Clears the values of the Temp and Humd array variables for the first time.
  for (byte i = 0; i < 2; i++) {
    Humd[i] = 0;
    Temp[i] = 0.00;
  }
  //---------------------------------------- 

  //---------------------------------------- Set Wifi to STA mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //---------------------------------------- 

  delay(100);

  //---------------------------------------- Connect to Wi-Fi (STA).
  Serial.println("------------");
  Serial.println("WIFI STA");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("------------");
  //:::::::::::::::::: 
  //---------------------------------------- 

  delay(500);

  //---------------------------------------- Handle Web Server
  Serial.println();
  Serial.println("Setting Up the Main Page on the Server.");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });
  //---------------------------------------- 

  //---------------------------------------- Handle Web Server Events
  Serial.println();
  Serial.println("Setting up event sources on the Server.");
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 10 second
    client->send("hello!", NULL, millis(), 10000);
  });
  //---------------------------------------- 

  //---------------------------------------- Send a GET request to <ESP_IP>/set_LED?Slave_Num=<inputMessage1>&LED_Num=<inputMessage2>&LED_Val=<inputMessage3>
  server.on("/set_LED", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //:::::::::::::::::: 
    // GET input value on <ESP_IP>/set_LED?Slave_Num=<inputMessage1>&LED_Num=<inputMessage2>&LED_Val=<inputMessage3>
    // PARAM_INPUT_1 = inputMessage1
    // PARAM_INPUT_2 = inputMessage2
    // PARAM_INPUT_3 = inputMessage3
    // Slave_Number = PARAM_INPUT_1
    // LED_Number = PARAM_INPUT_2
    // LED_Value = PARAM_INPUT_3
    //:::::::::::::::::: 
    
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3)) {
      Slave_Number = request->getParam(PARAM_INPUT_1)->value();
      LED_Number = request->getParam(PARAM_INPUT_2)->value();
      LED_Value = request->getParam(PARAM_INPUT_3)->value();

      String Rslt = "Slave : " + Slave_Number + " || LED : " + LED_Number + " || Set to : " + LED_Value;
      Serial.println();
      Serial.println(Rslt);
      send_Control_LED = true;
    }
    else {
      send_Control_LED = false;
      Slave_Number = "No message sent";
      LED_Number = "No message sent";
      LED_Value = "No message sent";
      String Rslt = "Slave : " + Slave_Number + " || LED : " + LED_Number + " || Set to : " + LED_Value;
      Serial.println();
      Serial.println(Rslt);
    }
    request->send(200, "text/plain", "OK");
  });
  //---------------------------------------- 

  //---------------------------------------- Adding event sources on the Server.
  Serial.println();
  Serial.println("Adding event sources on the Server.");
  server.addHandler(&events);
  //---------------------------------------- 

  //---------------------------------------- Starting the Server.
  Serial.println();
  Serial.println("Starting the Server.");
  server.begin();
  //---------------------------------------- 

  // Calls the Rst_LORA() subroutine.
  Rst_LORA();

  Serial.println();
  Serial.println("------------");
  Serial.print("ESP32 IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Visit the IP Address above in your browser to open the main page.");
  Serial.println("------------");
  Serial.println();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis/Timer to send messages to slaves every 1 second (see interval_SendMSG_to_GetData variable).
  //  Messages are sent every one second is alternately.
  //  > Master sends message to Slave 1, delay 1 second.
  //  > Master sends message to Slave 2, delay 1 second.
  
  unsigned long currentMillis_SendMSG_to_GetData = millis();
  
  if (currentMillis_SendMSG_to_GetData - previousMillis_SendMSG_to_GetData >= interval_SendMSG_to_GetData) {
    previousMillis_SendMSG_to_GetData = currentMillis_SendMSG_to_GetData;

    Slv++;
    if (Slv > 2) Slv = 1;
    
    //:::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
    if (Slv == 1) {
      Humd[0] = 0;
      Temp[0] = 0.00;
      sendMessage("", Destination_ESP32_Slave_1, get_Data_Mode);
    }
    //:::::::::::::::::: 
    
    //:::::::::::::::::: Condition for sending message / command data to Slave 2 (ESP32 Slave 2).
    if (Slv == 2) {
      Humd[1] = 0;
      Temp[1] = 0.00;
      sendMessage("", Destination_ESP32_Slave_2, get_Data_Mode);
    }

  }

  if (finished_Sending_Message == true && finished_Receiving_Message == true) {
    if (send_Control_LED == true) {
      delay(250);
      send_Control_LED = false;
      if (Slave_Number == "S1") {
        Message = "";
        Message = LED_Number + "," + LED_Value;
        sendMessage(Message, Destination_ESP32_Slave_1, led_Control_Mode);
      }
      if (Slave_Number == "S2") {
        Message = "";
        Message = LED_Number + "," + LED_Value;
        sendMessage(Message, Destination_ESP32_Slave_2, led_Control_Mode);
      }
      delay(250);
    }
  }
  
  unsigned long currentMillis_RestartLORA = millis();
  
  if (currentMillis_RestartLORA - previousMillis_RestartLORA >= interval_RestartLORA) {
    previousMillis_RestartLORA = currentMillis_RestartLORA;

    count_to_Rst_LORA++;
    if (count_to_Rst_LORA > 30) {
      LoRa.end();
      Rst_LORA();
    }
  }
  //----------------------------------------

  //----------------------------------------parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
 
}
