#include <XBee.h>

#define ADDRESS_HI 0x13A200
#define ADDRESS_LO 0x414EA6F0
#define BAUD       9600
#define DELAY_ACK  500
#define DELAY_LOOP 800
#define PL_SIZE    2

uint8_t              payload [ PL_SIZE ];

XBee                 xbee     = XBee();
XBeeResponse         response = XBeeResponse();
XBeeAddress64        address  = XBeeAddress64 ( ADDRESS_HI, ADDRESS_LO );
ZBExplicitRxResponse rx       = ZBExplicitRxResponse();
ZBExplicitTxRequest  tx       = ZBExplicitTxRequest ( address, payload, sizeof ( payload ) );
ZBTxStatusResponse   txStatus = ZBTxStatusResponse();

void setup()
{
  Serial.begin ( BAUD );
  xbee.setSerial ( Serial );
}

void loop()
{
  /* Reset the payload to remove residual values if received data is shorter. */
  for ( uint8_t byteNo = 0; byteNo < PL_SIZE; byteNo++ )
  {
    payload [ byteNo ] = 0;
  }

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

      Serial.print   ( "    [ " );

      for ( uint8_t byteNo = 0; byteNo < PL_SIZE && byteNo < rx.getDataLength(); byteNo++ )
      {
        payload [ byteNo ] = rx.getData ( byteNo );

        Serial.print ( payload [ byteNo ], HEX );
        Serial.print ( " " );
      }

      Serial.println ( "]" );
      Serial.print   ( "    " );

      xbee.send ( tx );

      Serial.print ( "TX: " );

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
    }
    else
    {
      Serial.println ( "unknown" );
    }
  }

  Serial.println();

  delay ( DELAY_LOOP );
}
