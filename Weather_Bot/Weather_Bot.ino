// include the library code:
#include <LiquidCrystal.h>
#include <dht_nonblocking.h>
#include <TimedAction.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#include <IRremote.h>
#include <MFRC522.h>
#include <string.h>

#define LED_GREEN 22
#define LED_RED 23
#define BUZZER_PIN 4

#define IR_RECEIVER 3

// RFID Pins
#define RST_PIN   5     // Configurable, see typical pin layout above
#define SS_PIN    53   // Configurable, see typical pin layout above

// Temp sensor
static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// IR sensor
IRrecv irrecv(IR_RECEIVER);
decode_results results; 

// LCD display
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// ask for weather data
void get_weather_func() {
  Serial.println("req_weather");
}

void test_func() {
  Serial.println("TEST");
}

// Timed Actions
TimedAction getWeatherAction = TimedAction(300000, get_weather_func);
TimedAction testAction = TimedAction(5000, test_func);


String data = "ERR";
String uid = "";
String temp;
bool runOnce = true;
boolean rfid_state = true;
boolean lock_state = false;
int rfid_toggle = 0;
int awake_time = 0;
int result = -100000;
char awake[5];

void setup() {
  //16x2 LCD
  lcd.begin(16, 2);
  lcd.print("Hello, World!");

  irrecv.enableIRIn();


  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_GREEN, HIGH);

  Serial.begin(9600);
  SPI.begin();  
  mfrc522.PCD_Init();

  //strcpy(awake, "awake");
  delay(3000);
}

/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}

// get temp from sensor
static int get_temp_f() {
  float temperature;
  float humidity;
  
  /* Measure temperature and humidity.  If the functions returns
  true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true ) {
    int temp = (int)(((temperature * 9.0) / 5.0) + 32.0);
    return temp;
  } else {
    return -1000;
  }
}


// RFID

String valid_uid = "a369c218";

void rfid_func() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {
    Serial.print(F("Card UID:"));
    uid = "";
    String temp = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      //Serial.print(mfrc522.uid.uidByte[i], HEX);
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
      //strcat(uid, temp);
    } 
    Serial.print(uid);
    Serial.println();
 

    if (uid == valid_uid) {
      for(int i=0;i<80;i++)
      {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        lock_state = false;
        
       /*
        servo.write(90);
        digitalWrite(BUZZER_PIN,HIGH);
        delay(1);//wait for 1ms
        digitalWrite(BUZZER_PIN,LOW);
        delay(1);//wait for 1ms
        */
      }
    } else {
      for(int i=0;i<100;i++)
      {
        
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        lock_state = true;
        /*
        digitalWrite(BUZZER_PIN,HIGH);
        delay(2);//wait for 2ms
        digitalWrite(BUZZER_PIN,LOW);
        delay(2);//wait for 2ms
        */
      }
    }
    mfrc522.PICC_HaltA();
  } else {
    if (rfid_toggle > 10) {
      rfid_toggle = 0;
    } else {
      rfid_toggle++;
    }
  }
}

TimedAction rfidAction = TimedAction(100, rfid_func);




// IR

// Power button
boolean lcd_status = true;
void power_button() {
  if (lcd_status) {
    lcd.clear();
    lcd_status = false;
  } else {
    lcd_status = true;
  }
}

// Lock button
void stop_button() {
  lock_state = true;
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
}




void translateIR() // takes action based on IR code received

// describing Remote IR codes 

{
  switch(results.value)

  {
  case 0xFFA25D: Serial.println("POWER"); power_button(); break;
  case 0xFFE21D: Serial.println("FUNC/STOP"); stop_button(); break;
  case 0xFF629D: Serial.println("VOL+"); break;
  case 0xFF22DD: Serial.println("FAST BACK");    break;
  case 0xFF02FD: Serial.println("PAUSE");    break;
  case 0xFFC23D: Serial.println("FAST FORWARD");   break;
  case 0xFFE01F: Serial.println("DOWN");    break;
  case 0xFFA857: Serial.println("VOL-");    break;
  case 0xFF906F: Serial.println("UP");    break;
  case 0xFF9867: Serial.println("EQ");    break;
  case 0xFFB04F: Serial.println("ST/REPT");    break;
  case 0xFF6897: Serial.println("0");    break;
  case 0xFF30CF: Serial.println("1");    break;
  case 0xFF18E7: Serial.println("2");    break;
  case 0xFF7A85: Serial.println("3");    break;
  case 0xFF10EF: Serial.println("4");    break;
  case 0xFF38C7: Serial.println("5");    break;
  case 0xFF5AA5: Serial.println("6");    break;
  case 0xFF42BD: Serial.println("7");    break;
  case 0xFF4AB5: Serial.println("8");    break;
  case 0xFF52AD: Serial.println("9");    break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");  break;  

  default: 
    Serial.println(" other button   ");

  }// End Case

} //END translateIR

TimedAction irReadAction = TimedAction(300, translateIR);




void loop() {

  // Initial get weather
  if (runOnce == true) {
    Serial.println("req_weather");
    runOnce = false;
  }


  // IR READ
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    irReadAction.check(); 
    irrecv.resume(); // receive the next value
  }

  // READ DATA
  getWeatherAction.check();
  if (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');
  }
  temp = data + "F";

  /*
  char input[5];
  strcpy(input, data.c_str());

  
  result = strcmp(input, awake);
  lcd.setCursor(0, 0);
  lcd.print(input);
  lcd.print("   ");
  lcd.print(result);
  lcd.print("                  ");
  
  if (result != 0) {
    temp = data + "F   ";
  } else {
    awake_time = millis();
  }

  if (awake_time - millis() < -50000) {
    temp = "ERROR    ";
  } else {
    
  }
  */
 
   

  // RFID
  if (rfid_state) {
    rfidAction.check();
  }
  

  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true && lcd_status == true)
  {
    //update internal temp when available
    lcd.setCursor(0, 0);
    lcd.print( "I:" );
    lcd.print( (((temperature * 9.0) / 5.0) + 32.0), 1 );
    lcd.print( "F");
    lcd.print (" H:" );
    lcd.print( humidity, 1 );
    lcd.print( "%" );
  }
  if (lcd_status == true) { 
    //always update this, data updates on its own
    lcd.setCursor(0, 1);
    lcd.print( "O:");
    lcd.print(temp);
    if (lock_state == true) {
      lcd.print("    LCKD");
    } else {
      lcd.print("        ");
    }
  }
}
