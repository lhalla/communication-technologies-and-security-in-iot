# LoRaWAN Exercise Notes

## Task 5
lmic.c:1435	processJoinAccept (void)

@1510 calls	aes_sessKeys(LMIC.devNonce-1, &LMIC.frame[OFF_JA_ARTNONCE], LMIC.nwkKey, LMIC.artKey)

lmic.c:193	aes_sessKeys (u2_t devnonce, xref2cu1_t artnonce, xref2cu1_t nwkkey, xref2cu1_t artkey)

```
processJoinAccept calls aes_sessKeys
```

## Task 6
lmic.c:1035	decodeFrame (void)

@1088 calls	aes_verifyMic(LMIC.nwkKey, LMIC.devaddr, seqno, /\*dn\*/1, d, pend)
	
lmic.c:147	aes_verifyMic ( xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2cu1_t pdu, int len)
	
```	
decodeFrame calls aes_verifyMic
```	

## Task 7
lmic.c:1615	buildDataFrame (void)

@1727 calls	aes_appendMic(LMIC.nwkKey, MIC.devaddr, LMIC.seqnoUp-1, /\*up\*/0, LMIC.frame, flen-4)
	
lmic.c:154	aes_appendMic (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2cu1_t pdu, int len)
	
```	
buildDataFrame calls aes_appendMic
```	

## Task 8
lmic.c:1818	buildJoinRequest (u1_t ftype)

@1826 calls	aes_appendMic0(d, OFF_JR_MIC)

lmic.c:162	aes_appendMic0 (xref2cu1_t pdu, int len)

```
buildJoinRequest calls aes_appendMic0
```

## Task 9
Replay protection starts at lmic.c:1096.
```
if( seqno < LMIC.seqnoDn ) {
```
