/*
  WiFiEsp test: ClientTest
  http://www.kccistc.net/
  작성일 : 2022.12.19
  작성자 : IoT 임베디드 KSH

  수정일 : 2023. 04.07
  작성자 : 이지명
*/

#include <SPI.h>
#include <MFRC522.h>
#include <WiFiEsp.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define WIFITX 2  
#define WIFIRX 3
#define AP_SSID "iot0"
#define AP_PASS "iot00000"
#define SERVER_NAME "10.10.141.75"
#define SERVER_PORT 5000
#define LOGID "LJM_ARD"
#define PASSWD "PASSWD"
SoftwareSerial wifiSerial(WIFIRX, WIFITX);
WiFiEspClient client;


#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc(SS_PIN, RST_PIN);


#define CMD_SIZE 100
#define ARR_CNT 5
LiquidCrystal_I2C lcd(0x27, 16, 2);
char sendId[10] = "LJM_sql";
char sendBuf[CMD_SIZE];
char input;
int cnt = 0;

void setup() {

  Serial.begin(9600);
  wifi_Setup();
  SPI.begin();
  mfrc.PCD_Init();
  lcd.init();
	lcd.backlight();

}

void loop() {
  
  if (client.available()) {
     socketEvent();
  }
  if (!client.connected()) {
    server_Connect();
  }
  int str_len;
  char rfidBuf[CMD_SIZE]={0};
  char inputBuf[CMD_SIZE]= {0};
  lcd.setCursor(0,0);
  lcd.print("Swipe Card");
  if (mfrc.PICC_IsNewCardPresent() && mfrc.PICC_ReadCardSerial()) {
    lcd.clear();
    sprintf(rfidBuf,"%d",mfrc.uid.uidByte[0]);
    Serial.println("예매 비밀번호입력");  
    lcd.setCursor(0,0);
    lcd.print("Reseve PW");
    while (Serial.available() == 0); // 시리얼 모니터에서 입력이 있을 때까지 대기  
    {  
      do{
        str_len = Serial.readBytesUntil('\n', inputBuf, CMD_SIZE);
      }while(str_len);
    }
    sprintf(sendBuf, "[LJM_sql]pass@%s@%s\r\n", inputBuf, rfidBuf);
    lcd.setCursor(0,1);
    lcd.print(inputBuf);
    Serial.print(sendBuf);
    client.write(sendBuf, strlen(sendBuf));
    client.flush();
    memset(inputBuf, 0, sizeof(inputBuf));
    cnt = 0;
  }
  delay(500);
}

void socketEvent()
{
  int i = 0;
  char * pToken;
  char * pArray[ARR_CNT] = {0};
  char recvBuf[CMD_SIZE] = {0};
  int len;
  len = client.readBytesUntil('\n', recvBuf, CMD_SIZE);
  client.flush();
  
  Serial.println(recvBuf);
  pToken = strtok(recvBuf,"[:@]");

  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[:@]");
  }

  if (strcmp(pArray[1], "correct")==0) 
  {
    lcd.clear();
    pArray[2][strlen(pArray[2]) - 1] = '\0';
    room_pw(pArray[2]);
  }else if (strcmp(pArray[1],"Decline\r")==0) 
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Decline");
    delay(3000);
  }
      
}
  
void wifi_Setup() {
  wifiSerial.begin(9600);
  wifi_Init();
  server_Connect();
}
void wifi_Init()
{
  do {
    WiFi.init(&wifiSerial);
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present");
    }
    else
      break;
  } while (1);

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(AP_SSID);
  while (WiFi.begin(AP_SSID, AP_PASS) != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(AP_SSID);
  }
 
  Serial.println("You're connected to the network");
  printWifiStatus();
}
int server_Connect()
{
  Serial.println("Starting connection to server...");

  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    Serial.println("Connect to server");
    client.print("["LOGID":"PASSWD"]");
  }
  else
  {
    Serial.println("server connection failure");
  }
}
void printWifiStatus()
{
  // print the SSID of the network you're attached to

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void room_pw(char* name){
  int room_len;
  char roomBuf[CMD_SIZE]= {0};
  int id;
  sendBuf[0] = '\0';  
  Serial.println("방 비밀번호입력(최대8자리)");  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Room PW,maximum8");
  while (Serial.available() == 0); // 시리얼 모니터에서 입력이 있을 때까지 대기  
  {  
    do{
      room_len = Serial.readBytesUntil('\n', roomBuf, CMD_SIZE);
    }while(room_len);
  }
  id = atoi(name);
  sprintf(sendBuf, "[LJM_sql]room@%s@%d\r\n",roomBuf,id);
  lcd.setCursor(0,1);
  lcd.print(roomBuf);
  client.write(sendBuf, strlen(sendBuf));
  client.flush();  
  delay(3000);
  lcd.clear();
}
