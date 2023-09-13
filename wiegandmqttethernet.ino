#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wiegand.h>
#include "parametres.h"
#include "topic.h"
#include "secours.h"   
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
WIEGAND wg;

void setup() {
  pinMode(boutonPin, INPUT_PULLUP);  // Configurer la broche du bouton en mode Pull-up
  pinMode(relaisPin, OUTPUT);  // Configurer la broche du relais en sortie
  pinMode (LEDCLAVIER,OUTPUT);
  pinMode (BUZZER,OUTPUT);
  digitalWrite(BUZZER, HIGH);
 digitalWrite(LEDCLAVIER, HIGH);  // Désactiver le relais au démarrage
wg.begin(WG_PIN_GREEN,WG_PIN_WHITE); 
  Serial.begin(9600);  // Initialiser la communication série

  Ethernet.begin(mac);  // Démarrer la connexion Ethernet avec l'adresse MAC
  delay(1000);  // Attendre la stabilité de la connexion
  if (Ethernet.localIP() == IPAddress(0, 0, 0, 0)) {
    Serial.println("Échec de la connexion Ethernet !");
  } else {
    Serial.println("Connexion Ethernet établie !");
  }

  mqttClient.setServer(mqttServer, mqttPort);  // Configurer le serveur MQTT
mqttClient.setCallback(callback);
}

void loop() {
 
  if (!mqttClient.connected()) {
    reconnectMQTT();  // Tenter de se reconnecter au serveur MQTT
  }
   mqttClient.loop();
{
  if (millis()-lastKey > PINTIMEOUT) {
    if (pin!="") {
      pin="";
      lastKey=millis();
      Serial.println("Pin timeout");
    }
  }
  
}
  

  // Autres actions à effectuer indépendamment de la connexion Ethernet ou MQTT
  // ...
handleWiegand();
boutonsortie();
  delay(100);  // Ajouter un court délai pour éviter les rebonds
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Tentative de connexion MQTT...");

    if (mqttClient.connect("ArduinoinClient", mqttUsername, mqttPassword)) {
      Serial.println("Connecté au serveur MQTT !");
      Serial.println("souscrit au topics");
      Serial.println(topicLED);
      Serial.println(topicBUZZER);
      mqttClient.subscribe(topicLED);
      mqttClient.subscribe(topicBUZZER);
    } else {
      Serial.print("Échec de la connexion MQTT. Code d'erreur : ");
      Serial.println(mqttClient.state());
      delay(5000);  // Attendre avant de retenter la connexion
    }
  }
}

 void boutonsortie (){if (onpilote){if (digitalRead(boutonPin) == LOW) {
    if (!etatRelais) {
      Serial.println("Bouton enfoncé !");
       mqttClient.publish(topicbouton, "0");
      digitalWrite(LEDCLAVIER, HIGH);  // Activer le relais
      etatRelais = true;
    }
  } else {
    if (etatRelais) {
      Serial.println("Bouton relâché !");
      digitalWrite(LEDCLAVIER, LOW);  // Désactiver le relais
        mqttClient.publish(topicbouton, "0");
      etatRelais = false;
    }
  }}
  else if (!onpilote){if (digitalRead(boutonPin) == LOW) {
    if (!etatbouton) {
      Serial.println("Bouton enfoncé !");
       mqttClient.publish(topicbouton, "1");
      etatbouton = true;
    }
  } else {
    if (etatbouton) {
      Serial.println("Bouton relâché !");  
        mqttClient.publish(topicbouton, "0");
      etatbouton = false;
    }
  }}}
void handleWiegand() {
  if(wg.available()) {
    unsigned long wcode = wg.getCode();
    int wtype = wg.getWiegandType();
    Serial.print("Wiegand HEX = ");
    Serial.print(wcode,HEX);
    Serial.print(", DECIMAL = ");
    Serial.print(wcode);
    Serial.print(", Type W");
    Serial.print(wtype); 
 if ((pin == codeclamerde )) 
 { Serial.print (" C'est la merde je deverouille la porte");
       digitalWrite(LEDCLAVIER, HIGH);
      Serial.println("Relay1 off"); 
      delay(delaisortie);
      digitalWrite(LEDCLAVIER, LOW);
      Serial.println("Relay1 ON"); 
 }
    // RFID card was scanned  
    if ((wtype==26)||(wtype==34)) {
      String msg;
      msg = "{\"code\":";
      msg += wcode;
      msg += ",\"type\": \"rfid\"}";
      if (millis()-lastRfid > RFIDLIMIT) {
        rfidcount++;
        mqttClient.publish(topicEvent, msg.c_str());
        Serial.print("  -> MQTT sent");
        lastRfid = millis();
      } else {
        mqttClient.publish(topicEvent, "{ \"type\": \"rfidratelimit\" }");
        Serial.print("  -> RATELIMITED");
      }
    }

    // Keypad was used
    if (wtype==8) {
      if (wcode==27) {
        pin+="*";
        lastKey = millis();
        Serial.print(" | PIN = ");
        Serial.print(pin);
      }
      if (wcode==13) {
        lastKey = millis();
        String msg;
        msg = "{\"code\": \"";
        msg += pin;
        msg += "\" ,\"type\": \"pin\"}";
        Serial.print(" | PIN = ");
        Serial.print(pin);
        if (millis()-lastPin > PINLIMIT) {
          pincount++;
          mqttClient.publish(topicEvent, msg.c_str());
          Serial.print("  -> MQTT sent");
          pin="";
          lastPin = millis();
        } else {
          mqttClient.publish(topicEvent, "{ \"type\": \"pinratelimit\" }");
          Serial.print("  -> RATELIMITED");
          pin="";
        }
      }
      if ((wcode>=0)&&(wcode<=9)) {
        pin+=wcode;
        lastKey = millis();
        Serial.print(" | PIN = ");
        Serial.print(pin);
      }
    }
    Serial.println(); 
  }}

  void callback(char* topic, byte* payload, unsigned int length) {
  // Réception de messages MQTT
  Serial.print("Message reçu [");
  Serial.print(topic);
  Serial.print("] : ");

  // Convertir le payload en chaîne de caractères
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Traitement des messages en fonction du topic
  if (strcmp(topic, topicLED) == 0) {
    if (message == "1") {
      digitalWrite(LEDCLAVIER, HIGH);  // Activer le relais 1
      Serial.println("led rouge");
      mqttClient.publish(topicetatled,"ROUGE");
    } else if (message =="0") {
      digitalWrite(LEDCLAVIER, LOW);  // Désactiver le relais 1
      Serial.println("led verte");
      mqttClient.publish(topicetatled,"VERT");
    }
  } else if (strcmp(topic, topicBUZZER) == 0) {
    if (message == "0") {
      digitalWrite(BUZZER, HIGH);  // Activer le relais 2
      Serial.println("Buzzer OFF");
      mqttClient.publish(topicetatbuzzer,"0");
    } else if (message == "1") {
      digitalWrite(BUZZER, LOW);  // Désactiver le relais 2
      mqttClient.publish(topicetatbuzzer,"1");
      Serial.println("Relais 2 désactivé !");
    }
  }
}

  
