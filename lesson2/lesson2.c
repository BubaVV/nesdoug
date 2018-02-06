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
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char Text_Position;


const unsigned char TEXT[]={
"Hello World!"};

const unsigned char PALETTE[]={
0x1f, 0x00, 0x10, 0x20
}; //	black, gray, lt gray, white

//	Prototypes
void Load_Text(void);
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Load_Palette(void);





void main (void) {
	//	turn off the screen
	All_Off();
	Load_Palette();
	Reset_Scroll();

	//	turn on screen
	All_On();
	
	while (1){ //	infinite loop
		while (NMI_flag == 0);	//	wait till NMI
		NMI_flag = 0;
		
		if (Frame_Count == 30){ //	wait 30 frames = 0.5 seconds
			Load_Text();
			Reset_Scroll();
			Frame_Count = 0;
		}
	}
}
	
//	inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
	
	
	
	
	
void Load_Text(void) {
	if (Text_Position < sizeof(TEXT)){
		PPU_ADDRESS = 0x21;				//	set an address in the PPU of 0x21ca
		PPU_ADDRESS = 0xca + Text_Position; //	about the middle of the screen
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
	}
}


void All_Off(void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}

	
void All_On(void) {
	PPU_CTRL = 0x90; //	screen is on, NMI on
	PPU_MASK = 0x1e; 
}

	
void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}

	
void Load_Palette(void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}
