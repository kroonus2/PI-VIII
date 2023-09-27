#include <ESP8266WiFi.h>
#include <DHT.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Auntenticação dos Fatores FireBase
#define FIREBASE_HOST "piviii-903c2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "7qHOx0IWAgacH3o8tArVpswA5lm8wkH45wZPZz4s"

//Dados da Rede Wifi
#define WIFI_SSID "?"
#define WIFI_PASSWORD "?"

//Definindo o pino do sensor de temperatura
#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Definindo o pino do sensor de Movimento Pir
#define Pir D1

//Definindo os pinos do relê
#define Relay01 D0
#define Relay02 D3
#define Relay03 D4
#define Relay04 D5
#define Relay05 D6
#define Relay06 D7
#define Relay07 D8
#define Relay08 3

//Criando variaveis de estado dos reles
bool Relay01_Status = false;
bool Relay02_Status = false;
bool Relay03_Status = false;
bool Relay04_Status = false;
bool Relay05_Status = false;
bool Relay06_Status = false;
bool Relay07_Status = false;
bool Relay08_Status = false;


//Conectando aos servidores NtpClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Recebendo o Pino do sensor de umidade
bool movimento;
int timestamp;
float tem = 0;
float hum = 0;
unsigned long tempoAnterior = millis();

//Função de Pegar o timestamp "Contagem de segundos após 01/01/1970"
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

//Iniciando as variáveis e conectando ao banco de dados
void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("\n.");
    delay(500);
  }

  //Setup Pins
  dht.begin();
  timeClient.begin();
  pinMode(Pir, INPUT);  //Define pino sensor como entrada

  pinMode(Relay01, OUTPUT);
  pinMode(Relay02, OUTPUT);
  pinMode(Relay03, OUTPUT);
  pinMode(Relay04, OUTPUT);
  pinMode(Relay05, OUTPUT);
  pinMode(Relay06, OUTPUT);
  pinMode(Relay07, OUTPUT);
  pinMode(Relay08, OUTPUT);


  Serial.println("");
  Serial.print("conectedo: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST);
  if (Firebase.failed()) Serial.println(Firebase.error());
}


//Função que faz a leitura dos Dados dos sensores sendo um deles o PV
void leituraDados() {
  tem = dht.readTemperature();  //Lendo a temperatura atráves do sensor
  hum = dht.readHumidity();
  timestamp = getTime();  //Recenbendo o tempo da função getTime();

  Relay01_Status = Firebase.getBool("Relay01");
  Relay02_Status = Firebase.getBool("Relay02");
  Relay03_Status = Firebase.getBool("Relay03");
  Relay04_Status = Firebase.getBool("Relay04");
  Relay05_Status = Firebase.getBool("Relay05");
  Relay06_Status = Firebase.getBool("Relay06");
  Relay07_Status = Firebase.getBool("Relay07");
  Relay08_Status = Firebase.getBool("Relay08");

  //digitalWrite(Relay01, Relay01_Status);
  //delay(500);
  //digitalWrite(Relay02, Relay02_Status);
  delay(500);
  digitalWrite(Relay03, Relay03_Status);
  delay(500);
  digitalWrite(Relay04, Relay04_Status);
  delay(500);
  digitalWrite(Relay05, Relay05_Status);
  delay(500);
  digitalWrite(Relay06, Relay06_Status);
  delay(500);
  digitalWrite(Relay07, Relay07_Status);
  delay(500);
  digitalWrite(Relay08, Relay08_Status);
}

void enviaLeituraDados() {

  String fireTemp = String(timestamp) + String(";") + String(tem, 2);  //Concatenando o timestamp com a temperatura lida
  String humTemp = String(timestamp) + String(";") + String(hum, 2);


  // delay(500);
  // Serial.print("\tTimeStamp --> ");  //Printando a data lida
  // Serial.println(timestamp);

  // Serial.print("\tAr --> ");
  // Serial.println(hum + String("%"));

  // Serial.print("\tTemperatura --> ");  //Printando a temperatura ambiente
  // Serial.println(tem);

  delay(500);
  Firebase.pushString("DHT22/Temperatura", fireTemp);  //Enviando a string de timestamp/temperatura
  delay(500);
  Firebase.pushString("DHT22/Humidade", humTemp);
}

void detectarMovimento() {
  movimento = digitalRead(Pir);
  delay(1000);
  if (movimento) {
    Serial.print("Movimento Detectado!\n");
    delay(200);
    Firebase.setBool("Movimento", true);
    // digitalWrite(Relay01, HIGH);
    // Firebase.setBool("Relay01", false);
    // delay(200);
    // digitalWrite(Relay02, LOW);
    // Firebase.setBool("Relay02", true);
  } else {
    Serial.print("----\n");
    delay(200);
    Firebase.setBool("Movimento", false);
    // digitalWrite(Relay01, LOW);
    // Firebase.setBool("Relay01", true);
    // delay(200);
    // digitalWrite(Relay02, HIGH);
    // Firebase.setBool("Relay02", false);
  }
}


void loop() {
  if (((millis() - tempoAnterior) % 60000) == 0) {
    enviaLeituraDados();
  } else if (((millis() - tempoAnterior) % 12000) == 0) {
    leituraDados();
  } else if (((millis() - tempoAnterior) % 2000) == 0) {
    detectarMovimento();
  }
}
