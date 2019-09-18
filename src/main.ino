/* Demo to see if there is a library I can use for interfacing with a MagiQuest wand
 * Robbie Nichols
 * 8/17/19
*/

#include <Wire.h> // display runs with I2C
#include <U8g2lib.h> //display
#include "IRremote.h" // to read the IR reciever

#include <SPI.h>  // rfid runs on SPI
#include <MFRC522.h> // rfid card reader

//define ingredient rfid tags UIDs
uint32_t STIR = 1990940738;
uint32_t HEAT = 1454921794;
uint32_t SHAKE = 2796247106;
uint32_t ing1 = 3870840898;
uint32_t ing2 = 3065534530;
uint32_t ing3 = 381245506;
uint32_t ing4 = 1447778882;
uint32_t ing5 = 2260228162;

#define ing1_name "Carnejian"
#define ing2_name "Elysian"
#define ing3_name "Lapsus"
#define ing4_name "Romany"
#define ing5_name "Tengu"

#define ing_num 6

#define SS_PIN 21
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

int RECV_PIN = 14;

IRrecv irrecv(RECV_PIN);

decode_results results;
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/15,/* data=*/4, /* reset=*/16);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16);
uint8_t cursor_loc = 10;
uint8_t current_ing_count = 0;
uint32_t card_uid;
uint32_t recipie[ing_num] = {ing1, ing2, ing3, STIR, ing4, HEAT};
uint32_t trial[ing_num] = {0,0,0,0,0,0};
bool status = 0;

void setup() {
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

  pinMode(25,OUTPUT);
  pinMode(33,OUTPUT);
  u8g2.begin();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
  u8g2.setCursor(0,cursor_loc);

  //SPI.begin();
  SPI.begin(18, 19, 23, SS_PIN); // sck, miso, mosi, ss (ss can be any GPIO)
  mfrc522.PCD_Init();	// Init MFRC522 card

  delay(2000);
  Serial.println("Recipie");  // print out the recipie for troubleshooting
  for (uint8_t i=0;i < ing_num;++i){
    Serial.println(recipie[i]);
  }
  Serial.println();
}

void loop() {

  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    status = 1;
    for (uint8_t i=0;i < ing_num;++i){
      Serial.println(trial[i]);
      Serial.println(recipie[i]);
      Serial.println();
      if (trial[i] != recipie[i]){
        Serial.println("Fail");
        status = 0; // 0 = fail
        break;
      }
    }

    if (status){
      success_display();
    } else{
      fail_display();
    }

    irrecv.resume(); // Receive the next value
  }
  // There is probably a better way to do this - I copied it.
  // Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
  card_uid = getID();

	// Dump debug info about the card. PICC_HaltA() is automatically called.
	//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  Serial.println(card_uid);

  if (card_uid == ing1){
    u8g2.println(ing1_name);
    cursor_loc = cursor_loc+10;
  } else if (card_uid == ing2){
    u8g2.println(ing2_name);
    cursor_loc = cursor_loc+10;
  } else if (card_uid == ing3){
    u8g2.println(ing3_name);
    cursor_loc = cursor_loc+10;
  } else if (card_uid == ing4){
    u8g2.println(ing4_name);
    cursor_loc = cursor_loc+10;
  } else if (card_uid == ing5){
    u8g2.println(ing5_name);
    cursor_loc = cursor_loc+10;
  } else if (card_uid == STIR){
    u8g2.println("Stir");
    cursor_loc = cursor_loc+10;
  } else if (card_uid == HEAT){
    u8g2.println("Heat");
    cursor_loc = cursor_loc+10;
  } else if (card_uid == SHAKE){
    u8g2.println("Shake");
    cursor_loc = cursor_loc+10;
  }
  if (current_ing_count >= ing_num){ // fail if too many ingredients
    fail_display();
  } else {
    trial[current_ing_count] = card_uid;
    ++current_ing_count;

    u8g2.setCursor(0,cursor_loc);
    u8g2.sendBuffer();
  }
/*
  digitalWrite(25,HIGH);
  delay(500);
  digitalWrite(25,LOW);
  delay(500);
  */
}

unsigned long getID(){
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

void fail_display(){
  u8g2.clearBuffer();
  u8g2.drawStr(20,40,"Potion Failed");
  u8g2.sendBuffer();

  current_ing_count = 0;
  cursor_loc = 10;
  u8g2.setCursor(0,cursor_loc);
  u8g2.clearBuffer();
}

void success_display(){
  u8g2.clearBuffer();
  u8g2.drawStr(20,40,"Potion Success");
  u8g2.sendBuffer();

  current_ing_count = 0;
  cursor_loc = 10;
  u8g2.setCursor(0,cursor_loc);
  u8g2.clearBuffer();
}