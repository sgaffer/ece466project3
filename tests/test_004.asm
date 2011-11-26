            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-122  ; decrement stack pointer beyond frame
            SET     R14   , #5           
            ADD     R11   , R14   , #0    
            SET     R15   , #5           
            ADD     R12   , R15   , #0    
            SET     R16   , #5           
            ADD     R13   , R16   , #0    
            ADD     R8    , R13   , #0    
            ADD     R17   , R5    , #-115 
            SET     R18   , #10          
            MUL     R19   , R18   , #1    
            ADD     R20   , R17   , R19   
            ADD     R9    , R20   , #0    
            SET     R21   , #5           
            ADD     R22   , R9    , #0    
            STR     R21   , R22   , #0    
            SET     R23   , #0           
            ADD     R8    , R23   , #0    
        L2: ADD     R24   , R8    , #0     ; enter for loop
            SET     R25   , #100         
            SUB     R26   , R24   , R25   
            BRn     R26   , L5          
            SET     R26   , #0           
        L5: NOP                            ; less-than label
            BRz     R26   , L4          
            ADD     R27   , R9    , #0    
            LDR     R28   , R27   , #0    
            SET     R29   , #10          
            ADD     R30   , R28   , R29   
            ADD     R31   , R11   , #0    
            ADD     R32   , R30   , R31   
            ADD     R33   , R5    , #-115 
            ADD     R34   , R8    , #0    
            MUL     R35   , R34   , #1    
            ADD     R36   , R33   , R35   
            STR     R32   , R36   , #0    
        L3: ADD     R37   , R8    , #0    
            SET     R38   , #1           
            ADD     R39   , R37   , R38   
            ADD     R8    , R39   , #0    
            BRA     L2                     ; backedge of for-loop
        L4: NOP                           
            ADD     R40   , R5    , #-115 
            SET     R41   , #10          
            MUL     R42   , R41   , #1    
            ADD     R43   , R40   , R42   
            LDR     R44   , R43   , #0    
            SET     R45   , #100         
            SUB     R46   , R44   , R45   
            NOTL    R47   , R46          
            BRz     R47   , L7           ; if-then-else branch, fall thru to then-stmts
            ADD     R48   , R5    , #-115 
            SET     R49   , #9           
            MUL     R50   , R49   , #1    
            ADD     R51   , R48   , R50   
            LDR     R52   , R51   , #0    
            SET     R53   , #100         
            SUB     R54   , R52   , R53   
            NOTL    R55   , R54          
            BRz     R55   , L9           ; if-then-else branch, fall thru to then-stmts
            SET     R56   , #0           
            ADD     R57   , R9    , #0    
            STR     R56   , R57   , #0    
            BRA     L8                    
        L9: SET     R58   , #1            ; else label
            ADD     R59   , R9    , #0    
            STR     R58   , R59   , #0    
        L8: NOP                            ; if-then-else label
            BRA     L6                    
        L7: NOP                            ; else label
        L6: NOP                            ; if-then-else label
            ADD     R60   , R9    , #0    
            LDR     R61   , R60   , #0    
            ADD     R62   , R11   , #0    
            ADD     R63   , R61   , R62   
            ADD     R64   , R12   , #0    
            ADD     R65   , R63   , R64   
            ADD     R66   , R13   , #0    
            ADD     R67   , R65   , R66   
            ADD     R68   , R9    , #0    
            STR     R67   , R68   , #0    
            ADD     R69   , R8    , #0    
            STR     R69   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #122   ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
