//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Adresse MAC de votre carte Ethernet pour le uno
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE  };;  // Adresse MAC de votre carte Ethernet pour le nano
IPAddress ip(192, 168, 0, 243);  // Adresse IP de votre carte Ethernet
const char* mqttServer = "192.168.0.103";
const int mqttPort = 1883;
const char* mqttUsername = "jeedom";
const char* mqttPassword = "jeedom";
const int boutonPin = 4;  // Broche du bouton
const int onpilote=false; // si on pilote une porte ou un portail on mets true sinon en mets false
const int relaisPin = 8;  // Broche du relais pour piloter une porte ou un portail 
#define LEDCLAVIER 7  // Pin du led du clavier 
#define BUZZER 5
#define RELAY3 10
#define RELAY4 11
#define ANALOGINPUT A0
#define ANALOGINPUT2 14
#define ANALOGINPUT3 15
#define WG_PIN_GREEN 2  // D0
#define WG_PIN_WHITE 3  // D1
unsigned long rfidcount = 0;
unsigned long pincount = 0;
const int PINTIMEOUT=2000;      // pin code entry timeout in milliseconds
#define PINLIMIT 2000         // limit how quickly pin codes can be sent
#define RFIDLIMIT 2000        // limit how quickly rfids can be scanned
bool etatRelais = false;
bool etatbouton = true;// État du relais (activé/désactivé)
unsigned long tempsReconnexionEthernet = 0;  // Temps écoulé depuis la dernière tentative de connexion Ethernet
const unsigned long intervalleReconnexionEthernet = 5000;  // Intervalle de temps entre les tentatives de connexion Ethernet en millisecondes (5 secondes dans cet exemple)
unsigned long lastKey, lastPin, lastRfid;
String pin = "";
