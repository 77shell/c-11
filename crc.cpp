/*****************************************************************
 *  Copyright (c) 2010-2018 Delta Products
 *
 *  THIS IS UNPUBLISHED PROPRIETARY TRADE SECRET SOURCE CODE OF
 *  Delta Products
 *
 *  The copyright above and this notice must be preserved in all copies of this
 *  source code.  The copyright notice above does not evidence any actual or
 *  intended publication of such source code.  This source code may not be
 *  copied, disclosed, distributed, demonstrated or licensed except as expressly
 *  authorized by Delta Products.
 ----------------------------------------------------------------
 *  Name          :  crc.cpp
 *  Author        :
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :
 *
 *
 *  History:    ysh 1-18-2010          Create
 *************************************************************/

#include <iostream>

class Crc {
public:
   static unsigned short calCRC16(const unsigned char *pData, unsigned int len, unsigned short crc=0);
};

const unsigned short crc16Table[] = {
   0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
   0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
   0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
   0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
   0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
   0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
   0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
   0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
   0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
   0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
   0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
   0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
   0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
   0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
   0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
   0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
   0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
   0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
   0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
   0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
   0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
   0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
   0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
   0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
   0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
   0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
   0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
   0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
   0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
   0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
   0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
   0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};


unsigned short
Crc::calCRC16(const unsigned char *pData, unsigned int len, unsigned short crc)
{
   while(len-- > 0)
      crc = ((crc >> 8) & 0xFF) ^ crc16Table[(crc ^ *pData++) & 0xFF];
   return crc;
}

unsigned char rfm_cfg_data[] = {
// 16 01 (開始計算新的CRC)   // 15505000x
// Non-existed object	    // 15404000x
// 16 02 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 03 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 04 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 05 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 06 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 07 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 08 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 09 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 0F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 10 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 11 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 12 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 13 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 14 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 15 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 16 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 17 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 18 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 19 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 1F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 20 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 21 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 22 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 23 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 24 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 25 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 26 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 27 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 28 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 29 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 2F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 30 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 31 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 32 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 33 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 34 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 35 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 36 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 37 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 38 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 39 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 3F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 40 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 41 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 42 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 43 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 44 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 45 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 46 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 47 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 48 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 49 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 4F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 50 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 51 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 52 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 53 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 54 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 55 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 56 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 57 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 58 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 59 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 5F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 60 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 61 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 62 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 63 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 64 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 65 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 66 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 67 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 68 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 69 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 6F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 70 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 71 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 72 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 73 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 74 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 75 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 76 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 77 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 78 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 79 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 7F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 80 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 81 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 82 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 83 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 84 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 85 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 86 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 87 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 88 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 89 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8A 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8B 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8C 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8D 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8E 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 8F 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 90 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 91 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 92 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 93 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 94 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 95 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 96 		    // 15505000x
   0x00, 0x64, 		    // 15405100x
// 16 97 		    // 15505000x
   0x02, 0x1C, 		    // 15405100x
// 16 98 		    // 15505000x
   0x02, 0x1C, 		    // 15405100x
// 16 99 		    // 15505000x
   0x00, 0x64, 		    // 15405100x
// 16 9A 		    // 15505000x
   0x00, 0x0A, 		    // 15405100x
// 16 9B 		    // 15505000x
   0x00, 0x05, 		    // 15405100x
// 16 9C 		    // 15505000x
   0x00, 0x1E, 		    // 15405100x
// 16 9D 		    // 15505000x
   0x00, 0x37, 		    // 15405100x
// 16 9E 		    // 15505000x
   0x01, 0xA4, 		    // 15405100x
// 16 9F 		    // 15505000x
   0x02, 0x44, 		    // 15405100x
// 16 A0 		    // 15505000x
   0x00, 0x48, 		    // 15405100x
// 16 A1 		    // 15505000x
   0x00, 0x19, 		    // 15405100x
// 16 A2 		    // 15505000x
   0x00, 0x0D, 		    // 15405100x
// 16 A3 		    // 15505000x
   0xFF, 0xF3, 		    // 15405100x
// 16 A4 		    // 15505000x
   0x00, 0x23, 		    // 15405100x
// 16 A5 		    // 15505000x
   0x00, 0x48, 		    // 15405100x
// 16 A6 		    // 15505000x
   0x00, 0x0A, 		    // 15405100x
// 16 A7 		    // 15505000x
   0x00, 0x32, 		    // 15405100x
// 16 A8 		    // 15505000x
   0x00, 0x37, 		    // 15405100x
// 16 A9 		    // 15505000x
   0x01, 0xF9, 		    // 15405100x
// 16 AA 		    // 15505000x
   0x02, 0x21, 		    // 15405100x
// 16 AB 		    // 15505000x
   0x01, 0xF4, 		    // 15405100x
// 16 AC 		    // 15505000x
   0x00, 0x5F, 		    // 15405100x
// 16 AD 		    // 15505000x
   0x01, 0xCC, 		    // 15405100x
// 16 AE 		    // 15505000x
   0x00, 0x05, 		    // 15405100x
// 16 AF 		    // 15505000x
   0x00, 0x1E, 		    // 15405100x
// 16 B0 		    // 15505000x
   0x00, 0xC8, 		    // 15405100x
// 16 B1 		    // 15505000x
   0x00, 0x1E, 		    // 15405100x
// 16 B2 		    // 15505000x
   0x00, 0x0A, 		    // 15405100x
// 16 B3 		    // 15505000x
   0x00, 0x0A, 		    // 15405100x
// 16 B4 		    // 15505000x
   0x01, 0xB3, 		    // 15405100x
// 16 B5 		    // 15505000x
   0x01, 0xA4, 		    // 15405100x
// 16 B6 		    // 15505000x
   0x01, 0xA4, 		    // 15405100x
// 16 B7 		    // 15505000x
   0x01, 0xA4, 		    // 15405100x
// 16 B8 		    // 15505000x
   0x01, 0xA4, 		    // 15405100x
// 16 B9 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BA 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BB 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BC 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BD 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BE 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 BF 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C0 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C1 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C2 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C3 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C4 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C5 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C6 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C7 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 C8 		    // 15505000x
   0x00, 0x00, 0x00, 0x01,    // 15405100x
// 16 C9 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CA 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CB 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CC 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CD 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CE 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 CF 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D0 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D1 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D2 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D3 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D4 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D5 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D6 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D7 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D8 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 D9 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DA 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DB 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DC 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DD 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DE 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 DF 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E0 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E1 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E2 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E3 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E4 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E5 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E6 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E7 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E8 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 E9 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 EA 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 EB 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 EC 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 ED 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 EE 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 EF 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F0 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F1 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F2 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F3 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F4 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F5 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F6 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F7 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F8 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 F9 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 FA 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 FB 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 FC 		    // 15505000x
// Non-existed object	    // 15404000x
// 16 FD 		    // 15505000x
// Non-existed object	    // 15404000x
};

/*
  Write CRC back to module
  ID: 15506000x   Len: 6    Data: 16 FE 00 00 2C 79 
  Obj-group: 0x16
  Obj-id: 0xFE
  CRC: 00-00-2C-79
*/

int
main(int argc, char *argv[])
{
   using namespace std;
   unsigned short crc {0};
   crc = Crc::calCRC16(rfm_cfg_data, sizeof(rfm_cfg_data), crc);
   cout << "CRC: " << hex << crc << endl;
}
