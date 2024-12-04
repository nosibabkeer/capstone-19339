#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "WE_6392D4";
const char* password = "underfoot";

// Define pins and constants
#define DHTPIN 4 // DHT22 data pin
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

#define HX710B_DATA_PIN 16 // HX710B data pin
#define HX710B_CLOCK_PIN 17 // HX710B clock pin

const float calibrationFactor = 0.02; // Calibration factor to convert raw HX710B reading to mmHg



// Initialize Telegram BOT
#define BOTtoken "6757223735:AAFoJzegnn4ik36Y5CNYFUIOcl1BcX8FRUQ" // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "5427356054"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Indicates when motion is detected
void setup() {
  Serial.begin(115200);
  
  // Initialize DHT22
  dht.begin();
  Serial.println("DHT22 initialized.");
  
  // Initialize HX710B pins
  pinMode(HX710B_DATA_PIN, INPUT);
  pinMode(HX710B_CLOCK_PIN, OUTPUT);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

// Function to read pressure from HX710B
long readHX710B() {
  long result = 0;
  
  // Wait until HX710B is ready
  while (digitalRead(HX710B_DATA_PIN) == HIGH);
  
  // Read 24-bit data from HX710B
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX710B_CLOCK_PIN, HIGH);
    delayMicroseconds(1);
    result = result << 1;
    digitalWrite(HX710B_CLOCK_PIN, LOW);
    if (digitalRead(HX710B_DATA_PIN)) {
      result++;
    }
  }
  
  // 25th pulse
  digitalWrite(HX710B_CLOCK_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(HX710B_CLOCK_PIN, LOW);

  return result;
}

void loop() {
  // Read temperature and humidity from DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Read and calibrate pressure from HX710B
  long rawPressure = readHX710B();
  float pressure_mmHg = rawPressure * calibrationFactor;
  
  // Print DHT22 readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(humidity);
    Serial.print(",");

  }
  
  // Print HX710B pressure reading
  Serial.println(pressure_mmHg/388);
  bot.sendMessage(CHAT_ID, "Mr Ahmed khairy", "");

  delay(30000); // Delay for next reading
}
