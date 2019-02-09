#include <XBee.h>

#define ADDRESS_HI 0x13A200
#define ADDRESS_LO 0x414EA67F
#define BAUD       9600
#define DELAY_ACK  500
#define DELAY_LOOP 1000
#define PL_SIZE    2
#define SENSOR_PIN A0

uint8_t             payload [ PL_SIZE ];

XBee                xbee     = XBee();
XBeeAddress64       address  = XBeeAddress64 ( ADDRESS_HI, ADDRESS_LO );
ZBExplicitTxRequest tx       = ZBExplicitTxRequest ( address, payload, sizeof ( payload ) );
ZBTxStatusResponse  txStatus = ZBTxStatusResponse();

void setup()
{
  pinMode ( SENSOR_PIN, INPUT );

  Serial.begin ( BAUD );
  xbee.setSerial ( Serial );
}

void loop()
{
  /* Pin A0 has a range of 0-1023, so store the result in an uint16. */
  uint16_t sensorValue = analogRead ( SENSOR_PIN );

  Serial.println ( "RX: measurement" );
  Serial.print   ( "    [ " );

  /* Send the measured value using little-endian notation. */
  for ( uint8_t byteNo = 0; byteNo < PL_SIZE; byteNo++ )
  {
    payload [ byteNo ] = sensorValue & 0xFF;
    sensorValue >>= 8;

    Serial.print ( payload [ byteNo ], HEX );
    Serial.print ( " " );
  }

  Serial.println ( "]" );
  Serial.print   ( "    " );

  xbee.send ( tx );

  Serial.println();
  Serial.print ( "TX: " );

  /* Check if the transmission was successful. */
  if ( xbee.readPacket ( DELAY_ACK ) )
  {
    if ( xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE )
    {
      xbee.getResponse().getZBTxStatusResponse ( txStatus );

      if ( txStatus.getDeliveryStatus() == SUCCESS )
      {
        Serial.println ( "SUCCESS" );
      }
      else
      {
        Serial.println ( "FAILURE" );
      }
    }
  }
  else
  {
    Serial.println ( "ERROR: No response" );
  }

  Serial.println();

  delay ( DELAY_LOOP );
}
