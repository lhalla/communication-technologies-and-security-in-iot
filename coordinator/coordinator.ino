#include <XBee.h>

#define BAUD       9600
#define DELAY_LOOP 600

XBee                 xbee     = XBee();
XBeeResponse         resp     = XBeeResponse();
ZBExplicitRxResponse rx       = ZBExplicitRxResponse();
ZBTxStatusResponse   txStatus = ZBTxStatusResponse();

void setup()
{
  Serial.begin ( BAUD );
  xbee.setSerial ( Serial );
}

void loop()
{
  xbee.readPacket();

  if ( xbee.getResponse().isAvailable() )
  {
    Serial.print ( "RX: " );

    if ( xbee.getResponse().getApiId() == ZB_EXPLICIT_RX_RESPONSE )
    {
      Serial.print ( "payload: " );

      if ( rx.getOption() == ZB_PACKET_ACKNOWLEDGED )
      {
        Serial.println ( "ACK" );
      }
      else
      {
        Serial.println ( "NACK" );
      }

      xbee.getResponse().getZBRxResponse ( rx );

      Serial.print ( "    " );

      uint16_t sensorValue = 0;

      for ( int byteNo = 0; byteNo < rx.getDataLength(); byteNo++ )
      {
        sensorValue += ( rx.getData ( byteNo ) << byteNo * 8 );
      }

      Serial.print ( sensorValue );
      Serial.println();
    }
    else
    {
      Serial.println ( "unknown" );
    }
  }

  Serial.println();
 
  delay ( DELAY_LOOP );
}
