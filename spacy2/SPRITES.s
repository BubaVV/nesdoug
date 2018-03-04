; ------------------ ;
; all sprite objects ;
; ------------------ ;



Address_Animation:
.word Ship_Reg, Ship_Left, Ship_Right, Ship_Exp_1, Ship_Exp_2, Ship_Exp_3
.word Enemy1_Data, Enemy2_Data, Enemy3_Data, Enemy4_Data, Enemy_Explode, Pause_Data, Game_Over_Data, Victory_Data, $0000

;number of tiles
;all y coordinates, relative
;all tiles
;all attributes
;all x coordinates, relative
;all x flipped, relative


; each animation recipe goes like this...
; number of sprites
; y, tile, attrib, x - tile 1
; ... - tile 2, etc.
; then a list of all the flipped x's, if flipped


Ship_Reg:

.byte $06
.byte $00, $00, $00, $00
.byte $00, $00, $40, $08
.byte $08, $10, $00, $00
.byte $08, $10, $40, $08
.byte $10, $20, $00, $00
.byte $10, $20, $40, $08

;flipped x's
;.byte $08, $00, $08, $00, $08, $00
;never flipped



Ship_Left:
.byte $06
.byte $00, $01, $00, $00
.byte $00, $02, $00, $08
.byte $08, $11, $00, $00
.byte $08, $12, $00, $08
.byte $10, $21, $00, $00
.byte $10, $22, $00, $08
;these don't have flipped states, omitted


Ship_Right:
.byte $06
.byte $00, $02, $40, $00
.byte $00, $01, $40, $08
.byte $08, $12, $40, $00
.byte $08, $11, $40, $08
.byte $10, $22, $40, $00
.byte $10, $21, $40, $08
;these don't have flipped states, omitted


Ship_Exp_1: ;hero explosion
.byte $6
.byte $0, $30, $0, $0
.byte $0, $31, $0, $8
.byte $8, $40, $0, $0
.byte $8, $41, $0, $8
.byte $10, $50, $0, $0
.byte $10, $51, $0, $8
;these don't have flipped states, omitted


Ship_Exp_2:
.byte $9
.byte $0, $32, $0, $0
.byte $0, $33, $0, $8
.byte $0, $34, $0, $10
.byte $8, $42, $0, $0
.byte $8, $43, $0, $8
.byte $8, $44, $0, $10
.byte $10, $52, $0, $0
.byte $10, $53, $0, $8
.byte $10, $54, $0, $10
;these don't have flipped states, omitted

Ship_Exp_3:
.byte $9
.byte $0, $35, $0, $0
.byte $0, $36, $0, $8
.byte $0, $37, $0, $10
.byte $8, $45, $0, $0
.byte $8, $46, $0, $8
.byte $8, $47, $0, $10
.byte $10, $55, $0, $0
.byte $10, $56, $0, $8
.byte $10, $57, $0, $10
;these don't have flipped states, omitted

Enemy1_Data:
.byte $4
.byte $0, $3, $0, $0
.byte $0, $4, $0, $8
.byte $8, $13, $0, $0
.byte $8, $14, $0, $8
.byte 8,0,8,0 ;flipped x

Enemy2_Data:
.byte $4
.byte $0, $5, $0, $0
.byte $0, $6, $0, $8
.byte $8, $15, $0, $0
.byte $8, $16, $0, $8
.byte 8,0,8,0 ;flipped x

Enemy3_Data:
.byte $4
.byte $0, $7, $0, $0
.byte $0, $9, $0, $8
.byte $8, $17, $0, $0
.byte $8, $19, $0, $8
.byte 8,0,8,0 ;flipped x

Enemy4_Data:
.byte $4
.byte $0, $9, $0, $0
.byte $0, $a, $0, $8
.byte $8, $19, $0, $0
.byte $8, $1a, $0, $8
.byte 8,0,8,0 ;flipped x

Enemy_Explode:
.byte $4
.byte $0, $e, $0, $0
.byte $0, $f, $0, $8
.byte $8, $1e, $0, $0
.byte $8, $1f, $0, $8
.byte 8,0,8,0 ;flipped x
;shouldn't be flipped, but I added it anyway


Pause_Data:
.byte $5
.byte $0, $f0, $0, $0
.byte $0, $f1, $0, $8
.byte $0, $f2, $0, $10
.byte $0, $f3, $0, $18
.byte $0, $f4, $0, $20
;these don't have flipped states, omitted


Game_Over_Data:
.byte $8
.byte $0, $f5, $0, $0
.byte $0, $f1, $0, $8
.byte $0, $f6, $0, $10
.byte $0, $f4, $0, $18
.byte $0, $f7, $0, $28
.byte $0, $f8, $0, $30
.byte $0, $f4, $0, $38
.byte $0, $f9, $0, $40
;these don't have flipped states, omitted


Victory_Data:
.byte $8
.byte $0, $fa, $0, $0
.byte $0, $fb, $0, $8
.byte $0, $fc, $0, $10
.byte $0, $fd, $0, $18
.byte $0, $fe, $0, $20
.byte $0, $f9, $0, $28
.byte $0, $ee, $0, $30
.byte $0, $ef, $0, $38
;these don't have flipped states, omitted

