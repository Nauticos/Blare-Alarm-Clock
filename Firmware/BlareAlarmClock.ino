#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS 8
#define TFT_RST 7
#define TFT_DC 10
#define TFT_SCLK 4
#define TFT_MOSI 6
#define TFT_BL_PIN 20

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define NAV_UP_PIN 0      
#define NAV_DOWN_PIN 1    
#define ACTION_STOP_PIN 5 
#define BUZZER_PIN 3 

enum State { NORMAL, SET_ALARM, RINGING };
State currentState = NORMAL;

int current_hr = 12;
int current_min = 0;
int current_sec = 0;
unsigned long lastTimeUpdate = 0;

int alarm_hr = 0;
int alarm_min = 0;
int alarm_sec = 0;
bool alarm_enabled = false;

int temp_alarm_hr = 0;
int temp_alarm_min = 0;
int temp_alarm_sec = 0;
int setup_stage = 0; 
unsigned long lastActivityTime = 0;
const unsigned long SETUP_TIMEOUT_MS = 5000; 

unsigned long lastInteractionTime = 0;
const unsigned long BACKLIGHT_TIMEOUT_MS = 10000;
bool backlight_on = true;

bool last_nav_up = HIGH;
bool last_nav_down = HIGH;
bool last_action_stop = HIGH;

bool force_screen_update = true; 

void setup() {
  Serial.begin(115200);

  tft.init(284, 76);
  tft.setColRowStart(82, 18);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); 

  pinMode(NAV_UP_PIN, INPUT_PULLUP);
  pinMode(NAV_DOWN_PIN, INPUT_PULLUP);
  pinMode(ACTION_STOP_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TFT_BL_PIN, OUTPUT);
  digitalWrite(TFT_BL_PIN, HIGH); 

  display_ui("TIME", current_hr, current_min, current_sec);
  lastInteractionTime = millis();
}

void loop() {
  unsigned long current_millis = millis();

  if (current_millis - lastTimeUpdate >= 1000) {
    lastTimeUpdate += 1000;
    increment_time();

    if (currentState == NORMAL && backlight_on) {
      display_ui("TIME", current_hr, current_min, current_sec);
    }
  }

  bool nav_up_pressed = (digitalRead(NAV_UP_PIN) == LOW);
  bool nav_down_pressed = (digitalRead(NAV_DOWN_PIN) == LOW);
  bool action_stop_pressed = (digitalRead(ACTION_STOP_PIN) == LOW);

  if ((nav_up_pressed && !last_nav_up) || 
      (nav_down_pressed && !last_nav_down) || 
      (action_stop_pressed && !last_action_stop)) {
    
    lastInteractionTime = current_millis;
    lastActivityTime = current_millis;
    
    if (!backlight_on) {
      digitalWrite(TFT_BL_PIN, HIGH);
      backlight_on = true;
      force_screen_update = true;
      display_ui("TIME", current_hr, current_min, current_sec);
      delay(200); 
      return; 
    }
  }

  if (backlight_on && (currentState == NORMAL) && (current_millis - lastInteractionTime > BACKLIGHT_TIMEOUT_MS)) {
    digitalWrite(TFT_BL_PIN, LOW);
    backlight_on = false;
  }

  switch (currentState) {
    
    case NORMAL:
      if (nav_up_pressed && nav_down_pressed) {
        currentState = SET_ALARM;
        setup_stage = 0; 
        temp_alarm_hr = alarm_hr;
        temp_alarm_min = alarm_min;
        temp_alarm_sec = alarm_sec;
        
        force_screen_update = true;
        tft.fillScreen(ST77XX_BLACK);
        trigger_feedback_buzz();
        delay(1000); 
      }

      if (alarm_enabled && current_hr == alarm_hr && current_min == alarm_min && current_sec == alarm_sec) {
        currentState = RINGING;

        digitalWrite(TFT_BL_PIN, HIGH);
        backlight_on = true;
        
        force_screen_update = true;
        tft.fillScreen(ST77XX_BLACK); 
      }
      break;

    case SET_ALARM:
      if (nav_up_pressed && nav_down_pressed) {
        trigger_feedback_buzz();
        setup_stage++;
        
        if (setup_stage > 2) {
          alarm_hr = temp_alarm_hr;
          alarm_min = temp_alarm_min;
          alarm_sec = temp_alarm_sec;
          alarm_enabled = true;
          currentState = NORMAL;
          
          force_screen_update = true;
          tft.fillScreen(ST77XX_BLACK);
          display_ui("ALARM SAVED", alarm_hr, alarm_min, alarm_sec);
          delay(1000); 
          
          force_screen_update = true;
          tft.fillScreen(ST77XX_BLACK);
          display_ui("TIME", current_hr, current_min, current_sec);
        } else {
          force_screen_update = true;
          delay(1000); 
        }
      } 
      else if (nav_up_pressed && !last_nav_up) {
        if (setup_stage == 0) temp_alarm_hr = (temp_alarm_hr + 1) % 24;
        else if (setup_stage == 1) temp_alarm_min = (temp_alarm_min + 1) % 60;
        else if (setup_stage == 2) temp_alarm_sec = (temp_alarm_sec + 1) % 60;
        
        force_screen_update = true;
        delay(150);
      } 
      else if (nav_down_pressed && !last_nav_down) {
        if (setup_stage == 0) temp_alarm_hr = (temp_alarm_hr - 1 + 24) % 24;
        else if (setup_stage == 1) temp_alarm_min = (temp_alarm_min - 1 + 60) % 60;
        else if (setup_stage == 2) temp_alarm_sec = (temp_alarm_sec - 1 + 60) % 60;
        
        force_screen_update = true;
        delay(150);
      }

      if (millis() - lastActivityTime > SETUP_TIMEOUT_MS) {
        currentState = NORMAL;
        force_screen_update = true;
        tft.fillScreen(ST77XX_BLACK);
        display_ui("TIME", current_hr, current_min, current_sec);
      } else if (currentState == SET_ALARM) {
        display_ui("SET ALARM", temp_alarm_hr, temp_alarm_min, temp_alarm_sec, setup_stage);
      }
      break;

    case RINGING:
      if ((current_millis % 1000) < 500) { 
        tone(BUZZER_PIN, 1000); 
      } else {
        noTone(BUZZER_PIN);
      }
      
      display_ui("WAKE UP!", current_hr, current_min, current_sec);

      if (action_stop_pressed) {
        noTone(BUZZER_PIN);
        alarm_enabled = false; 
        currentState = NORMAL;
        lastInteractionTime = millis();
        
        force_screen_update = true;
        tft.fillScreen(ST77XX_BLACK);
        display_ui("ALARM OFF", alarm_hr, alarm_min, alarm_sec);
        delay(1000);
        
        force_screen_update = true;
        tft.fillScreen(ST77XX_BLACK);
        display_ui("TIME", current_hr, current_min, current_sec);
      }
      break;
  }

  last_nav_up = nav_up_pressed;
  last_nav_down = nav_down_pressed;
  last_action_stop = action_stop_pressed;
}

void trigger_feedback_buzz() {
  tone(BUZZER_PIN, 1000);
  delay(150);
  noTone(BUZZER_PIN);
}

void increment_time() {
  current_sec++;
  if (current_sec >= 60) {
    current_sec = 0;
    current_min++;
    if (current_min >= 60) {
      current_min = 0;
      current_hr++;
      if (current_hr >= 24) current_hr = 0;
    }
  }
}

void display_ui(const char* label, int h, int m, int s, int blink_stage = -1) {
  static char last_label[20] = "";
  static char last_time_buffer[15] = "";
  
  char hr_str[3], min_str[3], sec_str[3];
  sprintf(hr_str, "%02d", h);
  sprintf(min_str, "%02d", m);
  sprintf(sec_str, "%02d", s);

  if (blink_stage != -1) {
    bool show_blank = (millis() % 1000) < 500;
    if (blink_stage == 0 && show_blank) strcpy(hr_str, "  ");
    if (blink_stage == 1 && show_blank) strcpy(min_str, "  ");
    if (blink_stage == 2 && show_blank) strcpy(sec_str, "  ");
  }

  char time_buffer[15];
  sprintf(time_buffer, "%s:%s:%s", hr_str, min_str, sec_str);

  if (force_screen_update || strcmp(label, last_label) != 0) {
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print(label);
    tft.print("       ");
    strcpy(last_label, label);
  }

  if (force_screen_update || strcmp(time_buffer, last_time_buffer) != 0) {
    tft.setTextSize(4);
    tft.setCursor(10, 35);
    tft.print(time_buffer);
    strcpy(last_time_buffer, time_buffer);
  }
  
  force_screen_update = false; 
}