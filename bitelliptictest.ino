#include <panic.h>
#include <bignat.h>
#include <bigint.h>
#include <sha256new.h>
#include <sha256old.h>
#include <rmd160.h>
#include <base58.h>
#include <bitelliptic.h>
#include <bitaddress.h>

const int LEDPIN=LED_BUILTIN;

void PANIC(byte errorCode){
  Serial.print("Error: ");
  Serial.println(errorCode);
  for(;;){
    for(byte i=0;i<errorCode;i++){
      digitalWrite(LEDPIN,HIGH);
      delay(250);
      digitalWrite(LEDPIN,LOW);
      delay(250);
    }
    delay(1000);
  }
}

void PROGRESS(float progress){
//  static byte flipflop=0;
//  flipflop++;
//  digitalWrite(LEDPIN,flipflop%2);
  Serial.println(progress);
}

void printHash(uint8_t* hash, int length = 32) {
  uint8_t temp[256];
  memcpy(temp, hash, length);
  int i;
  for (i=0; i<length; i++) {
    Serial.print("0123456789abcdef"[temp[i]>>4]);
    Serial.print("0123456789abcdef"[temp[i]&0xf]);
  }
  Serial.println();
}

uint8_t *hexToByte(char mystring[]) {
  uint8_t* myuint = new uint8_t[128];
  for (uint8_t i=0; i<strlen(mystring)/2;i++) {
        myuint[i] = 0;
        for (uint8_t j=0; j<2; j++) {
            char firstchar = mystring[(i*2)+j];
            //printf("myuint[%d] = %3d mystring[%d+%d] = %c ", i, myuint[i], i, j, mystring[(i*2)+j]);
            if (firstchar >= '0' && firstchar <= '9') {
                //Serial.print("Number");
                myuint[i] = myuint[i]*16 + firstchar - '0';
            } else if (firstchar >= 'A' && firstchar <= 'F') {
                //Serial.print("LETTER");
                myuint[i] = myuint[i]*16 + firstchar - 'A' + 10;
            } else if (firstchar >= 'a' && firstchar <= 'f') {
                //Serial.print("letter");
                myuint[i] = myuint[i]*16 + firstchar - 'a' + 10;
            } else {
                // error
                Serial.println("Error");
            }
            //printf(" myuint[%d] = %3d\n", i, myuint[i]);
        }
    }
    return myuint;
}


int readBinaryString(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}



void setup(){
  Serial.begin(9600);

  Serial.println("Starting");

  char mystring2[] = "4c7a9640c72dc2099f23715d0c8a0d8a35f8906e3cab61dd3f78b67bf887c9ab";
  uint8_t *myuint2 = hexToByte(mystring2);
  uint8_t PRIVATE[32];
  memcpy(PRIVATE, myuint2, 32);
  Serial.print("Generated Private Key: ");
  printHash(PRIVATE, 32);

  WORD xbuf[32];
  WORD ybuf[32];

  struct bigint pubx(xbuf,sizeof(xbuf));
  struct bigint puby(ybuf,sizeof(ybuf));
  Serial.print("pubx: "); Serial.println(pubx._abs.len);
  bitaddress::generatePublicKey(PRIVATE,pubx,puby);

  Serial.print("pubx: "); Serial.println(pubx._abs.len);

  uint8_t tbuf[32];
  pubx._abs.Bytes(tbuf,sizeof(tbuf));
  Serial.print("pubx: "); printHash(tbuf, pubx._abs.len);
  //puby._abs.Bytes(tbuf,sizeof(tbuf));
  //Serial.print("puby: "); printHash(tbuf, puby._abs.len);

  
//  printpublic(pubx,puby);
  
}


void loop(){  
}
