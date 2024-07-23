#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Initialize LCD by associating LCD interface pins with the Arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize an array to store new NUID
byte nuidPICC[4];

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // Set up the LCD's number of columns and rows:
  SPI.begin(); // Initialize SPI communication
  rfid.PCD_Init(); // Initialize MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
 
  Serial.println(F("\nThis code scans and displays the UID in the following format:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println("\nHold your card to the reader...");
 
  lcd.print("Hold card to reader...");
  // Scroll 17 positions (string length) to the left to move text offscreen left:
  for (int positionCounter = 0; positionCounter < 17; positionCounter++) {
    delay(10);
    lcd.scrollDisplayLeft();
    delay(400);
  }
  delay(600);
  lcd.clear();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return; // Reset loop if no new card is present on sensor.

  if (!rfid.PICC_ReadCardSerial())
    return; // Verify if new card has been read

  // Check array to make sure the card is not one that has been scanned before
  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3]) {
     
    lcd.clear();
    lcd.print("New Card!");
    Serial.println(F("A new card has been detected."));
   
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    lcd.setCursor(0, 1);
    lcd.print("UID: ");
    Serial.println(F("The UID is:"));
   
    // Display UID on Serial Monitor
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.print(F("\nIn dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // Display UID on LCD
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10)
        lcd.print("0");
      lcd.print(rfid.uid.uidByte[i], HEX);
      if (i < rfid.uid.size - 1)
        lcd.print(":");
    }
   
  } else {
    Serial.println(F("Card read previously."));
    lcd.clear();
    lcd.print("Card read before");
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

// Helper routine to dump a byte array as hex values to Serial.
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

// Helper routine to dump a byte array as dec values to Serial.
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

// LCD RS pin to digital pin 7
// LCD Enable pin to digital pin 6
// LCD D4 pin to digital pin 5
// LCD D5 pin to digital pin 4
// LCD D6 pin to digital pin 3
// LCD D7 pin to digital pin 2
// LCD R/W pin to GND
// LCD VSS pin to GND
// LCD VCC pin to 5V
// LCD LED+ to 5V through a 220 ohm resistor
// LCD LED- to GND