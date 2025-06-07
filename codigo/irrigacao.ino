#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";      // Wi-Fi simulado do Wokwi
const char* password = "";
const char* mqtt_server = "test.mosquitto.org"; // Broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos
const int sensorPin = 34;  // Sensor de umidade (potenciômetro)
const int relePin = 2;     // LED (simula a bomba)

// Tópicos exclusivos para você
const char* mqtt_topic_umidade = "thais/irrigacao/umidade";
const char* mqtt_topic_status  = "thais/irrigacao/status";

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ThaisESP32Client")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH); // LED apagado
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int leitura = analogRead(sensorPin);
  int umidade = map(leitura, 4095, 0, 0, 100);

  // Publica a porcentagem de umidade
  client.publish(mqtt_topic_umidade, String(umidade).c_str());
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println("%");

  String status;

  if (umidade < 30) {
    digitalWrite(relePin, LOW); // Liga bomba (LED acende)
    status = "Solo seco - precisa de irrigação (umidade abaixo de 30%)";
  } else if (umidade < 60) {
    digitalWrite(relePin, HIGH); // Desliga bomba
    status = "Solo moderadamente úmido - irrigação não necessária (30% a 60%)";
  } else {
    digitalWrite(relePin, HIGH); // Desliga bomba
    status = "Solo úmido - irrigação suficiente (acima de 60%)";
  }

  // Publica o status
  client.publish(mqtt_topic_status, status.c_str());
  Serial.println(status);

  delay(5000); // Aguarda 5 segundos
}

  // Arquivo criado para o código do projeto de irrigação
