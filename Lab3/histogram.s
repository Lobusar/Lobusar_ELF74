THUMB

  PUBLIC EightBitHistogram
  SECTION .text: CODE
  
EightBitHistogram
    MOV R9, R3 ;store initial position (R9)
    MUL R6, R0, R1 ;store size (R6)
    CMP R6, #65536
    ITT GT ;if >64k 
      MOVGT R0, #0
      BXGT LR ;return 0
 ;preparation for clearHistogram
    MOV R7, #0
    MOV R8, #0
    PUSH {LR} ;put on stack
    BL clearHistogram
    POP {LR} ;clear  stack
 ;preparation for calculation
    MOV R7, #0
    PUSH {LR} ;put on stack
    BL calculateHistogram
    POP {LR} ;clear  stack
    MOV R0, R6 ;pick size for return
    BX LR
  
clearHistogram
  STR R8, [R3], #2 ;zeros the actual position and it`s offset  
  ADD R7, #1  
  CMP R7, #256 ;see if its the end
  BNE clearHistogram ;loop
  BX LR
   
calculateHistogram
  MOV R3, R9 ;get start position
  LDRB R8, [R2], #1;read the value an advance
 ;because the funcion is 8bits
 ;and the return is 16bits
 ;its necessary to double the value
 ;for a proper conversion
  ADD R8, R8 ; double the value
  LDR R11, [R3, R8] ;read the value of the actual position an its value
  ADD R11, #1 ;increment it
  ADD R3, R8
  STR R11, [R3] ;update the vector value position
  ADD R7, #1
  CMP R7, R6  ;see if its the end
  BNE calculateHistogram ;loop
  BX LR
 
  END