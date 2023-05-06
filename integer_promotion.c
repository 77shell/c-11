#include <stdio.h>

/*
  <Integer overflow>

     ffff'ffffh
 +)  7fff'ffffh
 ---------------
   1'7fff'fffeh

wraparound
   7fff'fffeh

 <char underflow>

     80h -> -1
 -)   1h
 ---------------
     7fh -> 127
*/

int
main(int argc, char *argv[])
{
	char c = -128;
	char uc = 127;
	char rc = c - 1; // underflow
	printf("char %d (%x)\n", rc, rc);

	short ssi = -1;
	unsigned short sui = 0x7fff;
	short sresult = ssi + sui; // promote to unsigned short
	printf("ssi %d (%x), sui %d (%x) short %d (%x)\n", ssi, ssi, sui, sui, sresult, sresult);
	
	signed int si = -1;
	unsigned int ui = 0x7fff'ffff;
	int result = si + ui;
	printf("si %d (%x), ui %d (%x), Interger promotion : %d (%x)\n", si, si, ui, ui, result, result);
}
