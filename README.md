# paranoidwallet

http://paranoidwallet.com

This is a trial to generate bitcoin wallets with smallest (2kb) Arduino UNO.

1. Generation is done via dice-rolls - 8 rolls of 12 cubes (6-dice) - totalling in 96 base-6 number string.
2. Then this string is turned into 240-binary array, and a nonce is added to turn it into 256-bits.
3. The 256-bit binary array is then hashed, turned into base58 and checked if it complies with Mini-Private key: https://en.bitcoin.it/wiki/Mini_private_key_format
4. If the key is correct - it is turned into bitcoin address: https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

Current problem is that base58 lib and uECC (ECDSA) lib arern't working together because of low memory issues.
So some code is commented.