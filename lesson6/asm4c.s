;asm funtions that can be called by the c code. Written by Doug Fraker 2015.


.importzp _joypad1, _joypad1old, _joypad1test, _joypad2, _joypad2old, _joypad2test
.export _Get_Input

.segment "CODE"

;due to dmc audio playback occasionally messing with input, it is best to read twice and compare them
_Get_Input:
	lda _joypad1
	sta _joypad1old
	lda _joypad2
	sta _joypad2old
	
	ldx #$01	;strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input2:		;get first read, store them as a test
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1test
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2test
	dey
	bne Get_Input2
	
GetInputFixBug:
	ldx #$01	;restrobe strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input3:		;read again, store them as joypads
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2
	dey
	bne Get_Input3
	
CompareInput:
	lda _joypad1
	cmp _joypad1test
	bne :+
	lda _joypad2
	cmp _joypad2test
	bne :+
	rts				;if same, done
	
:	lda _joypad1
	sta _joypad1test
	lda _joypad2
	sta _joypad2test

	jmp GetInputFixBug ;if different, reread
