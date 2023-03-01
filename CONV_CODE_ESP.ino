#include <EspMQTTClient.h>
#include <stepper.h>
#include <Adafruit_NeoPixel.h>
#include <arduino.h>

//Константы-состояния:
#define WAITING_STATE 0
#define RUNNING_STATE 1
#define SCANNING_STATE 2
#define SORTING_STATE 3
#define ERROR_STATE 4

//Константы команды:
#define START_CMD 0x0
#define STOP_CMD 0x1
#define NO_COLOR 'ERROR'

//Глобальные переменные:
int state; //                  - хранит код текущего состояния системы
bool conv_speed; //            - хранит код текущей скорости конвейера
bool cmd; //                   - хранит код команды, полученной с MQTT сервера
String selected = '0000'; //   - хранит значения цветов, полученные с MQTT в формате 'rgbw'
String currentColor; //           - хранит код цвета, который определила СТЗ


void connectMQTT(){
  EspMQTTClient client( 
    "MEAV_Wi-Fi", 
    "MEAV_THE_BEST", 
    "mqtt.by",  // MQTT Broker server ip 
    "'login'",   // Can be omitted if not neededя 
    "'password'",   // Can be omitted if not needed 
    "23124"      // Client name that uniquely identify your device 
  );
}

void setup() 
{
  connectMQTT();
}

bool getCmdMQTT() 
{
  String command = client.subscribe("/user/NESCR/start", [] (const String &payload); 
  return command;
}

string getRedMQTT() 
{
  String red = client.subscribe("/user/NESCR/red", [] (const String &payload); 
  return red;
}

string getGreenMQTT() 
{
  String green = client.subscribe("/user/NESCR/green", [] (const String &payload); 
  return green;
}

string getBlueMQTT() 
{
  String blue = client.subscribe("/user/NESCR/blue", [] (const String &payload); 
  return blue;
}

string getWhiteMQTT() 
{
  String white = client.subscribe("/user/NESCR/white", [] (const String &payload); 
  return white;
}

void runScan() {
  Serial.println('1')
}

void getScan() {
  String data = Serial.read();
  return data;
}

bool checkObject()
{
  return;
}

void convMove()
{ 
}

void sortLeft()
{ 
}

void sortRight()
{ 
}

void getMQTT() 
{
  cmd = getCmdMQTT();
  selected[0] = getRedMQTT();
  selected[1] = getGreenMQTT();
  selected[2] = getBlueMQTT();
  selected[3] = getWhiteMQTT();
}

void sendErrorMQTT() {
}

void waitingStateHandler() 
{
  while (cmd != START_CMD) 
  {
    state = RUNNING_STATE;
  }
}

void runningStateHandler() 
{
  while (!checkObject()) 
  {
    if (cmd != STOP_CMD) 
    {
      conv_speed = 1;
      convMove(conv_speed);
    } 
    else 
    {
      conv_speed = 0;
      convMove(conv_speed);
      state = WAITING_STATE;
      break;
    }
  }
  conv_speed = 0;
  convMove(conv_speed);
  state = SCANNING_STATE;
}

void scanningStateHandler() 
{
  if (cmd != STOP_CMD) 
  {
    scanRun();
    currentColor = getScan();

    if (currentColor == NO_COLOR) 
    {
      state = ERROR_STATE;
    } 
    else 
    {
      state = SORTING_STATE;
    }
  } 
  else 
  {
    state = WAITING_STATE;
  }
}

void sortingStateHandler()
{
  if (cmd != STOP_CMD) 
  {
    if (colorInSelected()) 
    {
      sortLeft();
    } 
    else 
    {
      sortRight();
    }
    state = RUNNING_STATE;
  } 
  else 
  {
    state = WAITING_STATE;
  }
}

void errorStateHandler() 
{
  sendErrorMQTT();
  while (cmd != START_CMD) {
  }
  state = RUNNING_STATE;
}

void loop() 
{
  getMQTT();

  switch (state) 
  {
    case WAITING_STATE: 
    {
        waitingStateHandler();
        break;
    }
    case RUNNING_STATE: 
    {
        runningStateHandler();
        break;
    }
    case SCANNING_STATE: 
    {
        scanningStateHandler();
        break;
    }
    case SORTING_STATE: 
    {
        sortingStateHandler();
        break;
    }
    case ERROR_STATE: 
    {
        errorStateHandler();
        break;
    }
  }
}