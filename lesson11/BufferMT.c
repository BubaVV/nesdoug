// fills buffer from metatile/collision map of room
// first pass = the 2 columns to the left, 1/2 attribute table

void Buffer_Tiles(void){
	index = 2; 	// will be skipping the top 2 metatiles, it's the HUD
	index2 = 0;
	index3 = 0;
	while (index < 15){ 
		if (Nametable_Plus == 0){ // drawing to the opposite screen that we're on
			temp = C_MAP2[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the metatile
		}
		else{
			temp = C_MAP[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the metatile
		}
		
		temp3 = MT_color[temp]; // get color, store temp
		
		temp <<= 2; // x 4
		
		BUFFER1[index2] = METATILES[temp]; // get the tl tile
		++temp;
		BUFFER2[index2] = METATILES[temp]; // get the tr tile
		
		++temp;
		++index2;
		BUFFER1[index2] = METATILES[temp]; // get the bl tile
		++temp;
		BUFFER2[index2] = METATILES[temp]; // get the br tile
		
		++temp;
		++index2;
		
		++index; // next round, difference is attribute table stuff
		
		if (Nametable_Plus == 0){ // read from 2nd map
			temp = C_MAP2[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the metatile
		}
		else{
			temp = C_MAP[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the metatile
		}
		
		temp3 += (MT_color[temp] << 4); // get color, store temp
		BUFFER5[index3] = temp3;
		
		temp <<= 2; // x 4
		
		BUFFER1[index2] = METATILES[temp]; // get the tl tile
		++temp;
		BUFFER2[index2] = METATILES[temp]; // get the tr tile

		++temp;
		++index2;
		BUFFER1[index2] = METATILES[temp]; // get the bl tile
		++temp;
		BUFFER2[index2] = METATILES[temp]; // get the br tile

		++index2;
		++index3;
		++index;
	}
}

// second pass = the 2 columns to the right, finish attribute table
void Buffer_Tiles2(void){
	index = 2; // will be skipping the top 2 metatiles, later
	index2 = 0;
	index3 = 0;
	while (index < 15){
		if (Nametable_Plus == 0){ // drawing to the opposite screen that we're on
			temp = C_MAP2[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the mt
		}
		else{
			temp = C_MAP[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the mt
		}
		
		temp3 = MT_color[temp]; // get palette #, store temp3
		
		temp <<= 2; // x 4
		
		BUFFER3[index2] = METATILES[temp]; // get the tl tile
		++temp;
		BUFFER4[index2] = METATILES[temp]; // get the tr tile
		
		++temp;
		++index2;
		BUFFER3[index2] = METATILES[temp]; // get the bl tile
		++temp;
		BUFFER4[index2] = METATILES[temp]; // get the br tile
		
		++temp;
		++index2;
		
		++index; // next round, difference is attribute table stuff
		
		if (Nametable_Plus == 0){ // read from 2nd map
			temp = C_MAP2[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the mt
		}
		else{
			temp = C_MAP[(Horiz_scroll_Plus>>4) + (index<<4)]; // get the mt
		}
		
		temp3 += (MT_color[temp] << 4); // get palette #, store temp3
		BUFFER6[index3] = temp3 << 2;
		
		temp <<= 2; // x 4
		
		BUFFER3[index2] = METATILES[temp]; // get the tl tile
		++temp;
		BUFFER4[index2] = METATILES[temp]; // get the tr tile

		++temp;
		++index2;
		BUFFER3[index2] = METATILES[temp]; // get the bl tile
		++temp;
		BUFFER4[index2] = METATILES[temp]; // get the br tile

		++index2;
		++index3;
		++index;
	}

	// finish putting together the attribute table buffer7
	for (index=0;index<8;++index){
		BUFFER7[index] = BUFFER5[index] + BUFFER6[index]; 
	}
}

