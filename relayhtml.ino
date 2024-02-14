#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "****nama Wifi***";
const char *password = "**PASS WIFI***";

AsyncWebServer server(80);

const int relayCount = 4;
bool relayState[relayCount] = { false, false, false, false };

const int relayPins[relayCount] = { 27, 26, 25, 33 };

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < relayCount; ++i) {
    pinMode(relayPins[i], OUTPUT);
  }
  // Menghubungkan ESP32 ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Alamat IP server: ");
  Serial.println(WiFi.localIP());
  // Mengatur route dan menangani permintaan GET dan POST
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, World!");
  });

  server.on("/relay", HTTP_POST, [](AsyncWebServerRequest *request) {
    String relayStateStr = request->arg("state");
    int relayNumber = request->arg("relay").toInt();

    // Mengubah status relay sesuai dengan perintah
    if (relayNumber >= 1 && relayNumber <= relayCount) {
      if (relayStateStr == "on") {
        relayState[relayNumber - 1] = true;
      } else if (relayStateStr == "off") {
        relayState[relayNumber - 1] = false;
      }

      // Mengirim respons ke klien
      request->send(200, "text/plain", "OK");

      // Mencetak pesan dari HTML ke Serial Monitor
      Serial.print("Menerima perintah dari HTML - Relay : ");
      Serial.print(relayNumber);
      Serial.print(" diatur ke ");
      Serial.println(relayStateStr);
    } else {
      // Mengirim respons jika nomor relay tidak valid
      request->send(400, "text/plain", "Nomor relay tidak valid");
    }
  });



  // Menjalankan server
  server.begin();
}

void loop() {


  // Periksa status relay dan kontrol perangkat keras sesuai
  for (int i = 0; i < relayCount; ++i) {
    // Implementasikan sesuai dengan kebutuhan perangkat keras (hardware) yang digunakan
    // relayState[i] menunjukkan apakah relay ke-(i+1) harus dinyalakan (true) atau dimatikan (false)
    digitalWrite(relayPins[i], relayState[i] ?  LOW : HIGH);
  }

  // Cek apakah ada data yang diterima dari relay dan cetak ke Serial Monitor
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    Serial.print("Menerima data dari relay: ");
    Serial.println(receivedData);
  }

  delay(100);  // Beri sedikit keterlambatan
}
