#include <panic.h>
#include <bignat.h>
#include <bigint.h>
#include <sha256new.h>
#include <sha256old.h>
#include <rmd160.h>
#include <base58.h>
#include <bitelliptic.h>
#include <bitaddress.h>
#include <uECC.h>


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
  static byte flipflop=0;
  flipflop++;
  digitalWrite(LEDPIN,flipflop%2);
//  Serial.println(progress);
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

void printprivate(uint8_t *PRIVATE){
  char encoded[64];
  
  uint8_t wifbuf[37];
  wifbuf[0]=0x80;
  memcpy(&wifbuf[1],PRIVATE,32);
  
  Sha256 sha;
  sha.init();
  sha.input(wifbuf, 33);
  uint8_t hash[32];
  memcpy(hash, sha.result(), 32);
  
  sha.reset();
  sha.init();
  sha.input(hash, 32);
  uint8_t hash2[32];
  memcpy(hash2, sha.result(), 32);
  
  wifbuf[33]=hash2[0];
  wifbuf[34]=hash2[1];
  wifbuf[35]=hash2[2];
  wifbuf[36]=hash2[3];
  
  base58::encode(wifbuf,sizeof(wifbuf),encoded,sizeof(encoded));
  Serial.print("WIF : "); Serial.println(encoded);
}

void printpublic(const struct bigint &x,const struct bigint &y){
  char encoded[40];
  bitaddress::generateAddress(x,y,encoded,sizeof(encoded)); 
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

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int readBinaryString(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}

bool isMiniKeyValid(char *miniKey) {
  if (strlen(miniKey) != 31) return false;

  Sha256 sha;
  sha.init();
  sha.input((uint8_t*) miniKey, 31);
  uint8_t *miniHash = sha.result();
//  printHash(miniHash);
  if (miniHash[0] == 0) return true;
  return false;
}

void hashMiniKey(uint8_t *miniKey, uint8_t *miniKeyHash) {
  Sha256 sha;
  sha.init();
  sha.input(miniKey, 30);
  uint8_t *result = sha.result();
  memcpy(miniKeyHash, result, 32);
}

void generateMiniKey(char *deccode, char *miniKey, unsigned long nonce) {
  Serial.println(deccode);
  memcpy(miniKey, "S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy", 31);
  Serial.println(miniKey);
  Serial.println(nonce);
}

void generateMiniKey2(char* deccode, char* miniKey, unsigned long nonce) {
  char myArray[6][4] = {"000", "001", "010", "011", "100", "101"};
  char myArrayShort[6][3] = {"00", "01", "10", "11", "10", "11"};
  uint8_t bytecode[96];
  char diceString[257];
  memset(&diceString, 0, 257);

  //Serial.println(deccode);

  int dicePtr = 0;
  
  for (int i=0; i<strlen(deccode); i++) {
    bytecode[i] = deccode[i]-'0';
    if (i%2 == 0) {
      memcpy(&diceString[dicePtr], &myArray[bytecode[i]], 3);
      dicePtr+=3;
    } else {
      memcpy(&diceString[dicePtr], &myArrayShort[bytecode[i]], 2);
      dicePtr+=2;
    }
  }



  //Serial.println(dicePtr);

  char s[33];
  char nonceString[17] = "0000000000000000";
  itoa(nonce, s, 2);
  memcpy(&nonceString[16-strlen(s)], &s, strlen(s));

  memcpy(&diceString[dicePtr], &nonceString, 16);
  //Serial.println(diceString);


  uint8_t diceHex[32];

//  Serial.print("RAW Dice Hex: "); 
  
  char testS[9] = "00000000";
  for (int i=0; i<32; i++) {
    memcpy(&testS, &diceString[i*8], 8);
    //Serial.println(testS);
    diceHex[i] = readBinaryString(testS);
    
//    Serial.print((strlen(itoa(diceHex[i],s,16))==1)?"0":""); Serial.print(itoa(diceHex[i],s,16));
  }

//  Serial.println();
  
  Sha256 sha;
  sha.init();
  sha.input(diceHex, 32);
  uint8_t *diceSha = sha.result();
//  Serial.print("SHA1: ");
//  printHash(diceSha, 32);


  char diceBase58[70];

  base58::encode(diceSha,32,diceBase58,60);

  memset(miniKey, 0, 32);
  miniKey[0] = 'S';
  memcpy(&miniKey[1], &diceBase58, 29);

}

void setup(){
  Serial.begin(9600);
  char deccode[97] = "205322530521114551133350525512211112430354434532012152503054530000545435014235014430013132544211";
  char miniKey[32];
  memset(&miniKey, '0', 32);
  miniKey[31] = 0;
  unsigned long nonce;

  Serial.println("Starting");

//  memcpy(&miniKey, "S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy?", 32);
//  Serial.print("B58: "); Serial.println(diceBase58);

  bool valid = false;
  
  for (nonce = 0; nonce<1000 & !valid; nonce++) {
    generateMiniKey(deccode, miniKey, nonce);
    miniKey[30] = '?';
    valid = isMiniKeyValid(miniKey);
//    Serial.print("MiniKey: "); Serial.print(miniKey);
//    Serial.print(" Valid: "); Serial.println(valid);
  }


  miniKey[30] = 0;
  
  Serial.print("MiniKey: "); Serial.println(miniKey);
  Serial.print("  Valid: "); Serial.println(valid);
  Serial.print("  Nonce: "); Serial.println(nonce);
  uint8_t miniKeyHash[32];
  hashMiniKey((uint8_t*) miniKey, miniKeyHash);

  Serial.print("   Hash: "); printHash(miniKeyHash);
  
  uint8_t PRIVATE[32];
  memcpy(PRIVATE, miniKeyHash, 32);

  //generatePrivateKey(PRIVATE);
  //char mystring[] = "18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725";
  //char mystring[] = "2e73f70cdc720552594f0f1e939920765961eccb7a4f6faf8bf0073923968c0a";
  //uint8_t *myuint = hexToByte(mystring);
  //memcpy(PRIVATE, myuint, 32);

  Serial.print("Generated Private Key: ");
  printHash(PRIVATE, 32);

  Serial.print("Generated WIF: ");
//  printprivate(PRIVATE);

  uint8_t private2[32];
  memcpy(private2, PRIVATE, 32);
  const struct uECC_Curve_t * curve = uECC_secp256k1();
  uint8_t public1[66];
  uint8_t tempPublic[66];
  uECC_compute_public_key(private2, tempPublic, curve);
  memcpy(public1, tempPublic,66);
  
  for (int i=65; i>0; i--) {
    public1[i] = public1[i-1];
  }
  public1[0] = 0x04;
  
  Serial.print("Generated Public Key: ");
  printHash(public1, 65);

  Sha256 sha;
  sha.init();
  sha.input(public1, 65);
  uint8_t sha1[32];
  memcpy(sha1, sha.result(), 32);
  Serial.print("SHA1: ");
  printHash(sha1, 32);
  uint8_t digest[25];
  struct digest d;
  Reset(&d);
  Write(&d,sha1,32);
  Sum(&d,digest+1);
  digest[0]=0;

  Serial.print("RIPEMD-160 + 0 Byte: "); printHash(digest, 21);

  sha.reset();
  sha.init();
  sha.input(digest, 21);
  uint8_t sha2[32];
  memcpy(sha2, sha.result(), 32);
  Serial.print("SHA2: ");
  printHash(sha2, 32);

  sha.reset();
  sha.init();
  sha.input(sha2, 32);
  uint8_t sha3[32];
  memcpy(sha3, sha.result(), 32);
  Serial.print("SHA3: ");
  printHash(sha3, 32);

  memcpy(&digest[21], sha3, 4);
/*  digest[21]=sha3[0];
  digest[22]=sha3[1];
  digest[23]=sha3[2];
  digest[24]=sha3[3];
*/

  Serial.print("RAW Address: ");
  printHash(digest, 25);
  


  char adrs[40];
  uint8_t *inp;
  inp = digest;
  base58::encode(inp,25,adrs,40);
  Serial.print("Bitcoin Address: "); Serial.println(adrs);
  Serial.println(freeRam());
  Serial.println("End");

}


void loop(){  
}
