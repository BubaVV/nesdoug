/*	simple Hello World, for cc65, for NES
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
unsigned char index;

const unsigned char TEXT[]={
"Hello World!"};

const unsigned char PALETTE[]={
0x1f, 0x00, 0x10, 0x20
}; //	black, gray, lt gray, white

	
	
	

void main (void) {
	//	turn off the screen
	PPU_CTRL = 0;
	PPU_MASK = 0;
	
	//	load the palette
	PPU_ADDRESS = 0x3f; 	//	set an address in the PPU of 0x3f00
	PPU_ADDRESS = 0x00;
	
	for(index = 0; index < sizeof(PALETTE); ++index){
		PPU_DATA = PALETTE[index];
	}
		
	//load the text
	PPU_ADDRESS = 0x21;  	//	set an address in the PPU of 0x21ca
	PPU_ADDRESS = 0xca;  	//	about the middle of the screen
	for( index = 0; index < sizeof(TEXT); ++index ){
		PPU_DATA = TEXT[index];
	}
		
	//	reset the scroll position	
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
	
	//	turn on screen
	PPU_CTRL = 0x90; 	//	screen is on, NMI on
	PPU_MASK = 0x1e;
	
	//	infinite loop
	while (1);
};
	
	