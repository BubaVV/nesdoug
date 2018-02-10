const unsigned char PALETTE[]={
0x11, 0x00, 0x00, 0x31, // blue
0x00, 0x00, 0x00, 0x15, // red
0x00, 0x00, 0x00, 0x27, // yellow
0x00, 0x00, 0x00, 0x1a, // green
}; 
// note, 11 is the default background color = blue

const unsigned char Attrib_Table[]={
0x44, // 0100 0100, 
0xbb, // 1011 1011, 
0x44, // 0100 0100, 
0xbb}; // 1011 1011

// 33221100 = 2 bits per attribute area
// 0 1		= their relative position on the screen
// 2 3
// each box is 16x16 pixels