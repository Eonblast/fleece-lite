**Eonblast Fleece 0.3.1**

Source Folder
-------------

#### Lua-Facing Files

    fleece-config.h       compile-time configurations
    
    fleece.c              main module file, includes the other c files  
    fleece.h              main header file, not relevant  
    
    fleece-lua.c          lua-facing Fleece functions (hello & json so far)  


#### Main Module Files
    
    fleece-intern.h       internal header file

    fleece-buffers.c      custom string buffer source  
    fleece-buffers.h
    
    fleece-insp_ctrl.c    traversal control structure  
    fleece-insp_ctrl.h      
    
    fleece-numbers.c      number to ascii conversions  
    fleece-size.c         JSON size pre-calculation  
    fleece-stringify.c    Lua data traversal and JSON string production  
    fleece-copy.c         JSON-optimized copying sources
    
    
#### Other Files
    
    Makefile             sub make file, used by the Makefile in Fleece root

    README.md            this file
    README.html          same as README.md in HTML format

    fleece.o             Fleece C compilation file, present after build
    fleece.so            Fleece C library, present after build
    
*Lua* headers are included from ../include/ 

For tests see the ../test/ folder.

## Escaping

If you use escaping, assembler *can* make a difference. Fleece does well for non-escaped strings with or without assembler.  But consider guarding your string contents against special characters yourself in any case, and do any necessary escaping yourself. In most cases this will prevent much double checking. The setting **E0** then ignores escaping issues entirely and is the fastest option for JSON conversion. It allows for strings being copied as they are, without looking at the individual characters.

## Assembler

Assembler helps most with strings that *are* to get their control characters escaped, as prescribed by the JSON standard. Assembler is implemented only for x86, using gcc inline/AT&T format. Switch assembler on by using linux-asm, linux-64-asm, or macosx-asm. The options *E1* - *E4* are the most affected.

The non-assembler parts are sufficiently fast and optimized for their task. Depending on architecture, they are only around 10 to 50% slower than assembler, mostly due to highly optimized C library memcpy functions. 

Obviously, when escaping characters, the built-in memcpy can not be used alone but string traversing has to take place. strcspn() turned out to be no help performance-wise. 

## Copy Strategies

There are multiple strategies visible in the fleece-copy.c source, some will be abandoned and cleaned up. Basically: 

#### Buffer Space
A) count first, check remaining buffer, enlarge if nec., then copy  
B) check if remaining buffer < 'max escape case' (6x), enlarge, copy   
C) without any checks rely on assured reserve buffer for single chars   

#### Size Prediction 
I) plain count, for finding the destination size, then copy  
II) count and copy, cancel copying when exceeding assured reserve  
III) use built in C lib strcspn to count escape characters (--)  

#### Copy Control
i) memory-copy using given source length  
ii) string-copy relying on final 0 (faster for ASM, not tru to Lua)  

#### Copying
1) use built in C lib memcpy  
2) use native C programmed memcpy (faster for old PPC) (--)  
3) use native C programmed, block pointer casting-memcpy  
4) use assembler (x86 only, 10 - 20% faster, more on older CPUs)  

___________________________________________________________________________
README fleece/src hd Feb 2011 - powered by Eonblast http://www.eonblast.com
