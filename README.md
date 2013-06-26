One Time Pad Utilities
======================

Glen Wiley <glen.wiley@gmail.com>

This is a set of command line utilities for creating and using one time pads
for encrypting and decrypting messages on common operating systems.

This tool set will seem awkward and inconvenient when you first begin using it, however it is useful to note that strong encryption is by its nature awkward.  
If you prefer convenience and can settle for breakable encrytpion then you are probably better off with a different tool set.

If you find yourself wondering whether it is worth the trouble, I recommend that you brush up on cryptography theory and compare one time pad encryption with other approaches.

This project is licensed under the Creative Commons [Attribution 3.0 Unported](http://creativecommons.org/licenses/by/3.0/) which means you can modify it and use it commercially but I ask that you provide appropriate attribution (give me credit).

Recommended Use Cases
=====================

It is best to generate your one time pad 

How to Encrypt
==============
1. Coordinate which sheet you will be using with the receiving party.
2. Write your clear text above the characters on the sheet.
3. For each character:
- Look up the clear text character on the column header in the matrix.
- Look up the key character on the row header in the matrix.
- The enrypted character is at the interesction of the column/row write that below the key character on your sheet.
- The characters under the key text are your encrypted message, send this to the receiving party.
4. Rejoice at the strength of your cryptography!

How to Decrypt
==============
1. Coordinate which sheet you will use with the sender.
2. Write the encrypted message BELOW the key characters on the sheet.
3. Look up the key character on the column header.
4. Find the encrypted character in that column.
5. Write the character in the row header above the key character.
6. The characters above the key characters are the decrypted message.

One Time Pad Encryption Theory
==============================

http://www.aspheute.com/english/20010924.asp
http://users.telenet.be/d.rijmenants/en/onetimepad.htm
http://www.scn.org/~bh162/one-time_pad_encryption.html

Random number generation
========================

TODO
====
* use autoconf/automake
* identify a CC license
* allow for alternative sources of entropy
