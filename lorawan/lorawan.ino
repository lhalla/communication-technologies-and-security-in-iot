/*********** TODO BEGIN ***********/
//#define ABP // Uncomment this line for ABP activation, leave as it is for OTA activation.

//In all sections that are marked with a TODO, replace "..." with your code.
/*********** TODO END *************/

#include <Wire.h>

// Cooking API libraries
#include <arduinoUART.h>
#include <arduinoUtils.h>

// LoRaWAN library
#include <arduinoLoRaWAN.h>

// Pin definition for error LED:
const int ERROR_LED =  13;

//////////////////////////////////////////////
const uint8_t socket = SOCKET1;
//////////////////////////////////////////////

/*********** TODO BEGIN ***********/
// Define device parameters for Back-End registration for ABP and OTAA
#ifdef ABP
char DEVICE_EUI[]  = "5EC0ABB010203062";
char DEVICE_ADDR[] = "5EC0ABB2";
char NWK_SESSION_KEY[] = "FBCE87F3FDDEB6BA78840BBF63159B3F";
char APP_SESSION_KEY[] = "8637540C05C929028D1E46729AFC4089";
#else //OTAA
char DEVICE_EUI[]  = "5EC07AA010203062";
char APP_EUI[] = "5EC0DE00ABB05EC0";
char APP_KEY[] = "FAD9F89BA660633B7A3EDBDC151DADE7";
#endif //ABP
/*********** TODO END *************/

// Define port to use in Back-End: from 1 to 223
const uint8_t PORT = 3;

/**
   Modes of operation.
   Altered by downlink application messages.
*/
const uint8_t LIGHT = 1;//send only the data from light sensor
const uint8_t TEMP = 2;//send only the data from temperature sensor
const uint8_t ALL = 3;//send data from all sensors

/**
   Holds current mode of operation for the node.
   Can be altered by the application using downlink messages.
*/
uint8_t nodeMode;

struct AppDataOut {
  uint8_t mode;
  float temp;
  uint8_t light;
};

struct AppDataIn {
  uint8_t mode;
};

void err_blink ( const uint8_t err_id, const uint8_t err_code ) {
  if ( err_code ) {
    digitalWrite ( ERROR_LED, HIGH );
    delay ( 2000 );
    digitalWrite ( ERROR_LED, LOW );
    delay ( 1000 );

    for ( uint8_t blinkNo = 0; blinkNo < err_id; blinkNo++ ) {
      digitalWrite ( ERROR_LED, HIGH );
      delay ( 250 );
      digitalWrite ( ERROR_LED, LOW );
      delay ( 250 );
    }

    digitalWrite ( ERROR_LED, HIGH );
    delay ( 2000 );
    digitalWrite ( ERROR_LED, LOW );
    delay ( 1000 );

    for ( uint8_t blinkNo = 0; blinkNo < err_code; blinkNo++ ) {
      digitalWrite ( ERROR_LED, HIGH );
      delay ( 500 );
      digitalWrite ( ERROR_LED, LOW );
      delay ( 500 );
    }
  }
}

void setup() {
  /*********** TODO BEGIN ***********/
  pinMode  ( ERROR_LED, OUTPUT );
  digitalWrite ( ERROR_LED, LOW );

  // Set default mode of operation as 'LIGHT'
  nodeMode = LIGHT;
  // Turn on the radio module using the appropriate library function. You need to specify socket.
  LoRaWAN.ON ( socket );
  /*********** TODO END *************/

  LoRaWAN.setADR((char*)"on");
  uint32_t freq = 867100000;
  for (uint8_t ch = 3; ch <= 12; ch++) {
    LoRaWAN.setChannelFreq(ch, freq);
    freq += 200000;
  }

  /*********** TODO BEGIN ***********/

#ifdef ABP
  // Set device parameters for Back-End registration (for ABP)
  err_blink ( 1, LoRaWAN.setDeviceEUI ( DEVICE_EUI ) );
  err_blink ( 2, LoRaWAN.getDeviceEUI() );
  err_blink ( 3, LoRaWAN.setDeviceAddr ( DEVICE_ADDR ) );
  err_blink ( 4, LoRaWAN.getDeviceAddr() );
  err_blink ( 5, LoRaWAN.setNwkSessionKey ( NWK_SESSION_KEY ) );
  err_blink ( 6, LoRaWAN.setAppSessionKey ( APP_SESSION_KEY ) );
  // Save device parameters to LoRaWAN module using the appropriate library function.
  err_blink ( 7, LoRaWAN.saveConfig() );
  // Perform ABP activation by calling the appropriate library function.
  err_blink ( 8, LoRaWAN.joinABP() );
#else //OTAA
  // Set device parameters for Back-End registration (for OTAA)
  err_blink ( 1, LoRaWAN.setDeviceEUI ( DEVICE_EUI ) );
  err_blink ( 2, LoRaWAN.getDeviceEUI() );
  err_blink ( 3, LoRaWAN.getDeviceAddr() );
  err_blink ( 4, LoRaWAN.setAppEUI ( APP_EUI ) );
  err_blink ( 5, LoRaWAN.setAppKey ( APP_KEY ) );
  // Save device parameters to LoRaWAN module using the appropriate library function.
  err_blink ( 6, LoRaWAN.saveConfig() );
  // Perform OTA activation by calling the appropriate library function.
  err_blink ( 7, LoRaWAN.joinOTAA() );
#endif //ABP
  /*********** TODO END *************/
}

void loop() {
  AppDataOut out;
  out.mode = nodeMode;
  /*********** TODO BEGIN ***********/
  switch (nodeMode) {
    case LIGHT:
      // Read a value from the light sensor into 'out'
      readLight ( out );
      break;
    case TEMP:
      // Read a value from the temperature sensor into 'out'
      readTemp ( out );
      break;
    case ALL:
      // Read a value from the light sensor into 'out'
      readLight ( out );
      // Read a value from the temperature sensor into 'out'
      readTemp ( out );
      break;
    default:
      //invalid mode of operation
      return;
  }
  sendData(out);
  AppDataIn in;
  if (receiveData(in)) {
    // Set mode of operation according to data received from AC.
    nodeMode = in.mode;
  }
  // Wait for 10 seconds!
  delay ( 10000 );
  /*********** TODO END *************/
}

/**
   Send uplink application data.
*/
void sendData(AppDataOut const& out) {
  String data = "";
  appendAsHex(out.mode, data);
  switch (out.mode) {
    case LIGHT:
      appendAsHex(out.light, data);
      break;
    case TEMP:
      appendAsHex(out.temp, data);
      break;
    case ALL:
      appendAsHex(out.light, data);
      appendAsHex(out.temp, data);
      break;
    default:
      //invalid mode of operation
      return;
  }
  data.toUpperCase();//libelium library accepts "DA7A", but not "da7a"
  /*
     '0' if OK
     '1' if error
     '2' if no answer
     '4' if data length error
     '5' if error when sending data
     '6' if module hasn't joined to a network
     '7' if input port parameter error
  */
  /*********** TODO BEGIN ***********/
  char char_data [ data.length() + 1 ];
  data.toCharArray ( char_data, data.length() + 1 );
  uint8_t retVal = LoRaWAN.sendConfirmed ( PORT, char_data ); // Send application data within a confirmed uplink message, by calling the appropriate library function. You need to specify the port.

  err_blink ( 11, retVal );
  /*********** TODO END *************/
}

/**
   Receive downlink application data.
*/
bool receiveData(AppDataIn& in) {
  if (LoRaWAN._dataReceived)
  {
    /**
       There's data on
       port number: LoRaWAN._port
       and Data in: LoRaWAN._data

       returned data is hex string (prepended with a nibble).
    */
    err_blink ( 12, 0 );
    uint8_t appData = LoRaWAN._data[1] - '0';//skip the prepended nibble at _data[0] and read from _data[1]
    switch (appData) {
      case LIGHT:
      case TEMP:
      case ALL:
        in.mode = appData;
        return true;
      default:
        //invalid mode of operation
        return false;
    }
  }
  return false;
}

void leftPad(uint8_t targetLen, char pad, String & s) {
  while (s.length() < targetLen) {
    s = pad + s;
  }
}

/**
   Converts 'val' to a hex string 'XX'.
   'val' will be represented with 2 HEX digits (1 byte) in total.
   Assumption: 0 <= val <= 100
   Examples:
   0 <-> '00'
   1 <-> '01'
   98 <-> '62'
   99 <-> '63'
   100 <-> '64'

   (Note: This may not be the best way of doing it, but it is simple to code.)
*/
void appendAsHex(uint8_t val, String & data) {
  String x = String(val, HEX);
  leftPad(2, '0', x);
  data += x;
}

/*
   Converts 'val' to a hex string 'SXXXYY' s.t.
    - S indicates sign, and is 1 if 'val' is negative, 0 otherwise.
    - XXX represents the part of 'val' before decimal point
    - YY represents the part of 'val' after decimal point
   'val' will be represented with 6 HEX digits (3 bytes) in total.
   Assumption: -1000.0 < val < 1000.0
   Examples:
   -987.65 <-> '13DB41'
   +987.65 <-> '03DB41'
   Note that 987 is (3DB) in base 16, and 65 is (41) in base 16.

   (Note: This may not be the best way of doing it, but it is simple to code.)
*/
void appendAsHex(float val, String & data) {
  /*********** TODO BEGIN ***********/
  // Implement this function.
  /* SIGN */
  data += ( val < 0 ) ? "1" : "0";
  val = fabs ( val );

  /* WHOLE PART */
  uint16_t tmp     = (uint16_t) val; // tmp is the whole part
  String   tmp_str = String ( tmp, HEX );
  leftPad ( 3, '0', tmp_str );
  data += tmp_str;

  /* DECIMALS */
  tmp = (uint16_t) ( 100 * ( val - tmp ) ); // tmp is the decimal part (up to two dp)
  tmp_str = String ( tmp, HEX );
  leftPad ( 2, '0', tmp_str );
  data += tmp_str;
  /*********** TODO END *************/
}

/**
   Reads a value from the light sensor in the range 0-100.
   0  <-> very dark
   100 <-> very bright

*/
void readLight(AppDataOut & out) {
  out.light = 50;
}

/**
   Reads a value from the temperature sensor in the range (-273.15)-(999.99).

*/
void readTemp(AppDataOut & out) {
  out.temp = 21.34f;
}
