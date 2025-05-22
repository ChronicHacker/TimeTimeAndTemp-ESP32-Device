<pre><code class="language-cpp">
#include &lt;WiFi.h&gt;
#include &lt;HTTPClient.h&gt;
#include &lt;ArduinoJson.h&gt;
#include &lt;TFT_eSPI.h&gt;

// ——— Wi-Fi credentials ———
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASS";

// ——— Timezone (Los Angeles) ———
const long  gmtOffset_sec      = -8 * 3600;
const int   daylightOffset_sec = 3600;

// ——— Weather fetch settings ———
const unsigned long fetchInterval = 10UL * 60UL * 1000UL; // 10 min cache
String weatherStr = "";
unsigned long lastFetch = 0;

// ——— Last values for coloring/advice ———
float currTemp = NAN;
int   currPop  = 0;

// ——— Display & touch ———
TFT_eSPI tft = TFT_eSPI();
uint16_t touchX, touchY;
bool showWeather = false;

// ——— Splash control ———
const unsigned long longPressTime = 3000; // 3 s
unsigned long touchStart = 0;

// ——— Prototypes ———
void showSplash();
void drawClockPageFull();
void drawWeatherPageFull();
void fetchWeather();

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));

  // Init display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  drawClockPageFull();

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Init NTP time
  configTime(gmtOffset_sec, daylightOffset_sec,
             "pool.ntp.org", "time.nist.gov");
}

void loop() {
  // Touch handling
  bool touched = tft.getTouch(&touchX, &touchY);
  if (touched && touchStart == 0) touchStart = millis();
  if (!touched && touchStart != 0) {
    unsigned long held = millis() - touchStart;
    touchStart = 0;
    if (held >= longPressTime) {
      // Long-press → splash
      showSplash();
      showWeather = false;
      tft.fillScreen(TFT_BLACK);
      drawClockPageFull();
      return;
    } else {
      // Tap → toggle
      showWeather = !showWeather;
      tft.fillScreen(TFT_BLACK);
      showWeather ? drawWeatherPageFull() : drawClockPageFull();
    }
  }

  // Auto-refresh
  static int lastMinute = -1;
  if (showWeather) {
    if (millis() - lastFetch > fetchInterval) {
      tft.fillScreen(TFT_BLACK);
      drawWeatherPageFull();
    }
  } else {
    struct tm ti;
    if (getLocalTime(&ti) && ti.tm_min != lastMinute) {
      tft.fillScreen(TFT_BLACK);
      drawClockPageFull();
      lastMinute = ti.tm_min;
    }
  }

  delay(1000);
}

// Splash screen (glitchy Chr0nicH@ck3r)
void showSplash() {
  const String msg = "Chr0nicH@ck3r";
  tft.setTextSize(3);
  int16_t bx = (tft.width() - tft.textWidth(msg)) / 2;
  int16_t by = (tft.height() - tft.fontHeight()) / 2;
  unsigned long endTime = millis() + 5000;
  while (millis() < endTime) {
    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i < 3; i++) {
      int16_t x = bx + random(-4, 5);
      int16_t y = by + random(-3, 4);
      uint16_t col = tft.color565(random(100, 256),
                                  random(0, 128),
                                  random(100, 256));
      tft.setTextColor(col, TFT_BLACK);
      tft.drawString(msg, x, y);
    }
    delay(200);
  }
}

// Clock page
void drawClockPageFull() {
  struct tm ti; getLocalTime(&ti);
  String greet;
  int h = ti.tm_hour;
  if      (h <  4) greet = "Goodnight";
  else if (h < 12) greet = "Good Morning";
  else if (h < 17) greet = "Good Afternoon";
  else if (h < 21) greet = "Good Evening";
  else             greet = "Goodnight";

  char timeBuf[9], dateBuf[20];
  strftime(timeBuf, sizeof(timeBuf), "%I:%M %p", &ti);
  strftime(dateBuf, sizeof(dateBuf), "%B %d, %Y", &ti);

  tft.setTextSize(2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  int16_t x = (tft.width() - tft.textWidth(greet)) / 2;
  tft.drawString(greet, x, 20);

  tft.setTextSize(5);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  x = (tft.width() - tft.textWidth(timeBuf)) / 2;
  int16_t y = (tft.height() - tft.fontHeight())/2 - 10;
  tft.drawString(timeBuf, x, y);

  tft.setTextSize(2);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  x = (tft.width() - tft.textWidth(dateBuf)) / 2;
  tft.drawString(dateBuf, x, y + tft.fontHeight() + 40);
}

// Weather page
void drawWeatherPageFull() {
  fetchWeather();

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  String title = "Weather in Los Angeles";
  int16_t x = (tft.width() - tft.textWidth(title)) / 2;
  tft.drawString(title, x, 5);

  String lines[3];
  int start = 0;
  for (int i = 0; i < 3; ++i) {
    int nl = weatherStr.indexOf('\n', start);
    lines[i] = weatherStr.substring(start,
                nl<0?weatherStr.length():nl);
    start = nl + 1;
  }

  tft.setTextSize(3);
  int h0 = tft.fontHeight() + 12;
  tft.setTextSize(2);
  int h1 = tft.fontHeight() + 12;
  int totalH = h0 + 2*h1;
  int16_t y0 = (tft.height() - totalH)/2;

  uint16_t tcol = TFT_CYAN;
  if      (currTemp >= 90) tcol = TFT_RED;
  else if (currTemp >= 80) tcol = TFT_ORANGE;
  else if (currTemp < 50)  tcol = TFT_BLUE;

  tft.setTextSize(3);
  tft.setTextColor(tcol, TFT_BLACK);
  x = (tft.width() - tft.textWidth(lines[0])) / 2;
  tft.drawString(lines[0], x, y0);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  x = (tft.width() - tft.textWidth(lines[1])) / 2;
  tft.drawString(lines[1], x, y0 + h0);

  x = (tft.width() - tft.textWidth(lines[2])) / 2;
  tft.drawString(lines[2], x, y0 + h0 + h1);

  String advice;
  if      (currPop >= 50) advice = "Better grab an umbrella.";
  else if (currTemp >= 90) advice = "Stay hydrated and cool.";
  else if (currTemp >= 80) advice = "Wear light clothing.";
  else if (currTemp < 50)  advice = "You might need a hoodie.";
  else                     advice = "Nice weather today.";

  tft.setTextSize(2);
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  int16_t aw = tft.textWidth(advice);
  tft.drawString(advice, (tft.width() - aw)/2, tft.height() - 20);
}

void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    weatherStr = "Temp = -- F\nRain = --%\nWind = -- mph";
    currTemp = NAN; currPop = 0;
    return;
  }
  HTTPClient http;
  String url =
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=34.0522&longitude=-118.2437"
    "&hourly=temperature_2m,precipitation_probability,wind_speed_10m"
    "&timezone=America%2FLos_Angeles"
    "&forecast_days=1"
    "&wind_speed_unit=mph"
    "&temperature_unit=fahrenheit"
    "&precipitation_unit=inch";
  http.begin(url);
  int code = http.GET();
  if (code != 200) {
    weatherStr = "Temp = -- F\nRain = --%\nWind = -- mph";
    currTemp = NAN; currPop = 0;
    http.end();
    return;
  }
  String pl = http.getString();
  http.end();
  StaticJsonDocument<2048> doc;
  if (deserializeJson(doc, pl)) {
    weatherStr = "Temp = -- F\nRain = --%\nWind = -- mph";
    currTemp = NAN; currPop = 0;
    return;
  }
  time_t now = time(nullptr);
  struct tm* ti = localtime(&now);
  char buf[20];
  strftime(buf,sizeof(buf),"%Y-%m-%dT%H:00", ti);
  String nowStr = buf;
  auto times = doc["hourly"]["time"].as<JsonArray>();
  auto temps = doc["hourly"]["temperature_2m"].as<JsonArray>();
  auto pops  = doc["hourly"]["precipitation_probability"].as<JsonArray>();
  auto ws    = doc["hourly"]["wind_speed_10m"].as<JsonArray>();
  float tVal=NAN; int pVal=0; float wVal=NAN;
  for (size_t i=0; i<times.size(); i++){
    if (times[i] == nowStr) { tVal=temps[i]; pVal=pops[i]; wVal=ws[i]; break; }
  }
  currTemp = tVal; currPop = pVal;
  if (isnan(tVal)) weatherStr = "Temp = -- F\nRain = --%\nWind = -- mph";
  else weatherStr = String("Temp = ")+String(tVal,1)+" F\n"
                +String("Rain = ")+String(pVal)+"%\n"
                +String("Wind = ")+String(wVal,1)+" mph";
  lastFetch = millis();
}
</code></pre>
