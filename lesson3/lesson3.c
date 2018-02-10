/*	simple Hello World, for cc65, for NES
 *	examples of attribute tables and pragmas and include
 *	doug fraker 2015
 *	feel free to reuse any code here
 */

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)

//	Globals
#pragma bss-name(push, "ZEROPAGE")
/*	forces these variables to be in the zeropage of the RAM
 *	note, in reset.s, I also changed the .import to .importzp
 *	zeropage code runs slightly faster than anything outside the zeropage
 */

unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char Text_Position;

#pragma bss-name(push, "BSS")
//	these variables to be in the BSS of the RAM, set as 0x300 in the cfg file
unsigned char test;


const unsigned char TEXT[]={
"Hello&World!"};

//	an easy way to clean up the code is to put seperate
//	functions in seperate files
#include "PALETTE.c"
#include "CODE.c"

// Prototypes
void Load_Text (void);





void main (void) {
	All_Off(); 		//	turn off screen
	Load_Palette();
	Reset_Scroll();
	All_On(); 		//	turn on screen
	while (1){ 		//	infinite loop
		while (NMI_flag == 0);	//	wait till NMI
		NMI_flag = 0;
		
		if (Frame_Count == 30){ //	wait 30 frames = 0.5 seconds
			Load_Text();
			Reset_Scroll();
			Frame_Count = 0;
			++test;	//	dummy, just making sure this compiles into the BSS section 0x300
		}
	} 
}
	
//	inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
	
	
	
	
void Load_Text (void) {
	if (Text_Position < sizeof(TEXT)){
		PPU_ADDRESS = 0x21;
		PPU_ADDRESS = 0xca + Text_Position; //	about the middle of the screen 21d0
		PPU_DATA = TEXT[Text_Position];
		
		PPU_ADDRESS = 0x21;
		PPU_ADDRESS = 0xea + Text_Position; //	one line down = add 0x20 to the low bit
		PPU_DATA = TEXT[Text_Position];
		
		++Text_Position;	
	}
	else {
		Text_Position = 0;
		PPU_ADDRESS = 0x21;
		PPU_ADDRESS = 0xca;
		for ( index = 0; index < sizeof(TEXT); ++index ){
			PPU_DATA = 0;	//	clear the text by putting tile #0 in its place
		}
		PPU_ADDRESS = 0x21;
		PPU_ADDRESS = 0xea;
		for ( index = 0; index < sizeof(TEXT); ++index ){
			PPU_DATA = 0;	//	clear the text by putting tile #0 in its place
		}
	}
}