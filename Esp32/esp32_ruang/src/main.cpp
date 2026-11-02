#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <EspMQTTClient.h>
// --- KONFIGURASI IDENTITAS KELOMPOK & RUANGAN ---
const char *KELOMPOK = "klpX"; // Ganti sesuai nomor kelompok
const char *ROOM = "ruang1";   // Ruang 1 memakai sensor AHT10 (I2C)
// --- PIN AKTUATOR LED & SENSOR AHT10 ---
#define LED1PIN 17
#define LED2PIN 27
#define I2C_SDA 19
#define I2C_SCL 22
Adafruit_AHTX0 aht;
bool ahtReady = false;
// --- KONFIGURASI WI-FI & BROKER LOKAL MOSQUITTO ---
EspMQTTClient client(
    "SSD_WIFI",      // Ganti dengan SSID Wi-Fi / Hotspot
    "PASWORD_WIFI",  // Ganti dengan Password Wi-Fi
    "IP_BROKER",     // Ganti dengan IPv4 Laptop Broker Mosquitto
    "USERNAME_MQTT", // Username MQTT (kosongkan atau isi sesuai konfigurasi broker)
    "PASWORD_MQTT",  // Password MQTT (kosongkan atau isi sesuai konfigurasi broker)
    "Client_ID",     // Client ID di-set di setup(), TIAP DEVICE DAN WEB HARUS UNIK, contoh: "esp32-klp4-ruang1"
    1883);           // Port default Mosquitto = 1883
String topicTemp, topicHum, topicLamp1, topicLamp2, topicStatus;
String topicLamp1State, topicLamp2State;
char clientIdBuffer[32];
unsigned long lastSend = 0;
void setup()
{
  Serial.begin(115200);
  pinMode(LED1PIN, OUTPUT);
  pinMode(LED2PIN, OUTPUT);
  digitalWrite(LED1PIN, LOW);
  digitalWrite(LED2PIN, LOW);
  // Inisialisasi I2C dan sensor AHT10
  Wire.begin(I2C_SDA, I2C_SCL);
  if (aht.begin())
  {
    ahtReady = true;
    Serial.println("[AHT10] Sensor AHT10 berhasil terdeteksi pada bus I2C!");
  }
  else
  {
    Serial.println("[AHT10] Gagal mendeteksi AHT10! Periksa kabel SDA(21) & SCL(22).");
  }
  // Pembentukan topik dinamis sesuai spesifikasi wajib + Nilai Tambah
  String prefix = String("pens/") + KELOMPOK + "/" + ROOM + "/";
  topicTemp = prefix + "temperature";
  topicHum = prefix + "humidity";
  topicLamp1 = prefix + "lamp1";
  topicLamp2 = prefix + "lamp2";
  topicStatus = prefix + "status";
  topicLamp1State = prefix + "lamp1/state";
  topicLamp2State = prefix + "lamp2/state";
  snprintf(clientIdBuffer, sizeof(clientIdBuffer), "esp32-%s-%s", KELOMPOK, ROOM);
  client.setMqttClientName(clientIdBuffer);
  // Set Keep-Alive 10 detik & Last Will and Testament (LWT)
  client.setKeepAlive(10);
  client.enableLastWillMessage(topicStatus.c_str(), "offline", true);
  client.enableDebuggingMessages();
}
void onConnectionEstablished()
{
  Serial.println("[MQTT] Tersambung ke Broker Lokal Mosquitto!");
  // 1. Publish status online (Retained = true) & state awal lampu
  client.publish(topicStatus, "online", true);
  client.publish(topicLamp1State, digitalRead(LED1PIN) ? "1" : "0", true);
  client.publish(topicLamp2State, digitalRead(LED2PIN) ? "1" : "0", true);
  // 2. Subscribe kontrol Lampu 1 + Kirim balik State Feedback (Nilai Tambah)
  client.subscribe(topicLamp1, [](const String &payload)
                   {
if (payload == "1") {
digitalWrite(LED1PIN, HIGH);
client.publish(topicLamp1State, "1", true);
} else if (payload == "0") {
digitalWrite(LED1PIN, LOW);
client.publish(topicLamp1State, "0", true);
} });
  // 3. Subscribe kontrol Lampu 2 + Kirim balik State Feedback (Nilai Tambah)
  client.subscribe(topicLamp2, [](const String &payload)
                   {
if (payload == "1") {
digitalWrite(LED2PIN, HIGH);
client.publish(topicLamp2State, "1", true);
} else if (payload == "0") {
digitalWrite(LED2PIN, LOW);
client.publish(topicLamp2State, "0", true);
} });
}
void loop()
{
  client.loop(); // Wajib dipanggil terus-menerus tanpa delay() panjang
  if (millis() - lastSend >= 5000)
  {
    lastSend = millis();
    if (!ahtReady)
    {
      ahtReady = aht.begin();
      return;
    }
    sensors_event_t humidityEvent, tempEvent;
    aht.getEvent(&humidityEvent, &tempEvent);
    float t = tempEvent.temperature;
    float h = humidityEvent.relative_humidity;
    if (isnan(t) || isnan(h))
    {
      Serial.println("[AHT10] Pembacaan sensor gagal (NaN)!");
      return;
    }
    client.publish(topicTemp, String(t, 1));
    client.publish(topicHum, String(h, 1));
    Serial.printf("[%s] AHT10 Sent -> Suhu: %.1f C | Kelembapan: %.1f %%RH\n", ROOM, t, h);
  }
}