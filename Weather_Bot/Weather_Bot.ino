// include the library code:
#include <LiquidCrystal.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
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


void loop() {
  

  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    lcd.setCursor(0, 0);
    lcd.print( "T = " );
    lcd.print( (((temperature * 9.0) / 5.0) + 32.0), 1 );
    lcd.print( " deg. F");
    lcd.setCursor(0, 1);
    lcd.print ("H = " );
    lcd.print( humidity, 1 );
    lcd.print( "%" );
  }
}
