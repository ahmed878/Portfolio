#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN       10
#define RST_PIN       9

#define ENCODER_CLK   2
#define ENCODER_DT    3

MFRC522 rfid(SS_PIN, RST_PIN);

byte authorizedUID[] = {0xF9, 0x51, 0xFD, 0xE4}; // Replace with your real UID

volatile int entryCount = 0;
volatile int exitCount = 0;

bool accessGranted = false;
bool cardScanned = false;
int lastCLK = HIGH;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();

    pinMode(ENCODER_CLK, INPUT_PULLUP);
    pinMode(ENCODER_DT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);

    Serial.println("🛡️ Système prêt. Scannez une carte...");
}

void loop() {
    if (!cardScanned) {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
            displayRFIDUID();

            if (isAuthorized(rfid.uid.uidByte, rfid.uid.size)) {
                Serial.println("✅ Carte autorisée. Tourniquet débloqué.");
                accessGranted = true;
                cardScanned = true;
            } else {
                Serial.println("❌ Carte non autorisée.");
                accessGranted = false;
            }

            rfid.PICC_HaltA();
            rfid.PCD_StopCrypto1();
        }
    }

    // Display counts regularly
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay > 5000) {
        displayCounts();
        lastDisplay = millis();
    }
}

void displayRFIDUID() {
    Serial.print("UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

bool isAuthorized(byte *uid, byte length) {
    for (byte i = 0; i < length; i++) {
        if (uid[i] != authorizedUID[i]) return false;
    }
    return true;
}

void readEncoder() {
    if (!accessGranted) return; // Only allow rotation after valid card

    int clkState = digitalRead(ENCODER_CLK);
    int dtState = digitalRead(ENCODER_DT);

    if (clkState != lastCLK) {
        if (dtState != clkState) {
            entryCount++;
            Serial.println("➡️  Tourner à Droite - Entrée détectée.");
        } else {
            exitCount++;
            Serial.println("⬅️  Tourner à Gauche - Sortie détectée.");
        }

        // After one valid rotation, reset access
        accessGranted = false;
        cardScanned = false;
        Serial.println("🔒 Tourniquet verrouillé. Veuillez scanner une nouvelle carte.");
    }

    lastCLK = clkState;
}

void displayCounts() {
    Serial.println("📊 --- Compteur de Passage ---");
    Serial.print("➡️  Entrées : ");
    Serial.println(entryCount);
    Serial.print("⬅️  Sorties : ");
    Serial.println(exitCount);
    Serial.println("-----------------------------");
}
