// ================= 腳位設定（沿用原接線） =================
const int RED_PIN    = 9;   // RGB LED 紅光腳
const int GREEN_PIN  = 10;  // RGB LED 綠光腳
const int BLUE_PIN   = 11;  // RGB LED 藍光腳
const int BUTTON_PIN = 2;   // 按鈕模組 Input 腳（按下為 LOW）

// ================= 狀態與時間參數 =================
// mode: 0=恆亮, 1=慢閃爍, 2=不快不慢閃爍, 3=快閃爍
int mode = 0; 

// 各閃爍模式的間隔時間（單位：毫秒 ms，可依需求微調）
const unsigned long SLOW_INTERVAL   = 1000; // 慢閃爍：1 秒
const unsigned long MEDIUM_INTERVAL = 400;  // 不快不慢：0.4 秒
const unsigned long FAST_INTERVAL   = 100;  // 快閃爍：0.1 秒

// 按鈕防彈跳變數
int lastReading = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 30;

// 閃爍計時變數
unsigned long previousMillis = 0;
bool ledOn = true;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT); // 外部上拉電阻模組

  // 初始狀態為恆亮
  setLedState(true);

  Serial.begin(9600);
  Serial.println("初始狀態：恆亮");
}

void loop() {
  checkButton();
  updateLed();
}

// ================= 偵測按鈕按一下切換狀態 =================
void checkButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      // 當按鈕被按下（從 HIGH 變成 LOW）時切換模式
      if (buttonState == LOW) {
        // 0 -> 1 -> 2 -> 3，按第 4 下回到 0（恆亮）
        mode = (mode + 1) % 4;

        // 切換模式瞬間重置計時並先點亮，視覺反應更即時
        ledOn = true;
        previousMillis = millis();
        printMode();
      }
    }
  }

  lastReading = reading;
}

// ================= 控制 LED 恆亮或不同頻率閃爍 =================
void updateLed() {
  if (mode == 0) {
    // 模式 0：恆亮
    setLedState(true);
    return;
  }

  // 依據目前模式決定閃爍間隔時間
  unsigned long interval = SLOW_INTERVAL;
  if (mode == 1) {
    interval = SLOW_INTERVAL;   // 慢閃爍
  } else if (mode == 2) {
    interval = MEDIUM_INTERVAL; // 不快不慢的閃爍
  } else if (mode == 3) {
    interval = FAST_INTERVAL;   // 快閃爍
  }

  // 達到間隔時間就反轉亮暗狀態
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    ledOn = !ledOn;
    setLedState(ledOn);
  }
}

// ================= 共陽極 RGB LED 亮暗控制 =================
void setLedState(bool on) {
  // 共陽極接 5V，因此輸出 LOW 為亮、HIGH 為暗
  int signal = on ? LOW : HIGH;
  digitalWrite(RED_PIN, signal);
  digitalWrite(GREEN_PIN, signal);
  digitalWrite(BLUE_PIN, signal);
}

// 序列埠監控視窗顯示目前狀態（方便檢查）
void printMode() {
  switch (mode) {
    case 0: Serial.println("目前狀態：恆亮"); break;
    case 1: Serial.println("目前狀態：慢閃爍"); break;
    case 2: Serial.println("目前狀態：不快不慢的閃爍"); break;
    case 3: Serial.println("目前狀態：快閃爍"); break;
  }
}