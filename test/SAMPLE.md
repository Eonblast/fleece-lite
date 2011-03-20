**Eonblast Fleece 0.3.1**

# Fleece Sample Benchmarks

## Fleece vs JSON4, LuaJSON C lib and Lua-Yajl

The result is given in nanoseconds per table entry ('ns/element'). The percentage shows Fleece' use of time compared to the other packages.

JSON4 is native Lua, LuaJSON and Lua-Yajl are C libraries. JSON4 and LuaJSON come bundled with Fleece. JSON4 runs out of the box, for LuaJSON use, e.g. 'make linux-test' to build it. Note that Lua-Yajl - not Fleece - caused problems on one Mac OS X. There is no support for Lua-Yajl here and it is not bundled, you need to build it on your own. Please use bench7.lua instead to make this exact benchmark but without Lua-Yajl.

This test was performed on a dual core 2.2 Ghz Ubuntu machine.

Below, you find the same test with Fleece compiled with the assembler option, in the concrete case: linux-64-asm. 
      
      lua test/bench74.lua

      Lua 5.1 official - Fleece 0.3.0
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=i                   
      1000x luajson.stringify(t)        710ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x json4.encode(t)            1960ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x yajl.to_string(t)          1210ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E0")         40ns/element   5%,   2%   3%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E1")         30ns/element   4%, 1.5% 2.4%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E2")         30ns/element   4%, 1.5% 2.4%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E3")         30ns/element   4%, 1.5% 2.4%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E4")         40ns/element   5%,   2%   3%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 10
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        360ns/element                 ["qt\nvyvluy","brol\"x","vdkfo\fh\n","buz","ksia","\\l","uv".. 
      1000x json4.encode(t)            2170ns/element                 ["qt\nvyvluy","brol\"x","vdkfo
                                                                                                    h\n","buz","ksia","\\l","uv",.. 
      1000x yajl.to_string(t)           640ns/element                 ["qt\nvyvluy","brol\"x","vdkfo\fh\n","buz","ksia","\\l","uv".. 
      1000x fleece.json(t, "E0")         60ns/element  16%, 2.7%   9%   ["qt
      vyvluy","brol"x","vdkfo
                             h
      ","buz","ksia","\l","uv","c",.. 
      1000x fleece.json(t, "E1")         80ns/element  22%,   3%  12%   ["qt
      vyvluy","brol*x","vdkfo
                             h
      ","buz","ksia","*l","uv","c",.. 
      1000x fleece.json(t, "E2")         80ns/element  22%,   3%  12%   ["qt
      vyvluy","brol\"x","vdkfo
                              h
      ","buz","ksia","\\l","uv","c.. 
      1000x fleece.json(t, "E3")         90ns/element  24%,   4%  14%   ["qt
      vyvluy","brol\"x","vdkfo
                              h
      ","buz","ksia","\\l","uv","c.. 
      1000x fleece.json(t, "E4")        110ns/element  30%,   5%  17%   ["qt\nvyvluy","brol\"x","vdkfo\fh\n","buz","ksia","\\l","uv".. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        580ns/element                 {"eyaoak":"tjlpfj","<wkkwlhn":"cc","wexgrx":"x\\hs","nqcjjew.. 
      1000x json4.encode(t)            2850ns/element                 {"eyaoak":"tjlpfj","<wkkwlhn":"cc","wexgrx":"x\\hs","nqcjjew.. 
      1000x yajl.to_string(t)          1120ns/element                 {"eyaoak":"tjlpfj","<wkkwlhn":"cc","wexgrx":"x\\hs","nqcjjew.. 
      cepy":"vvryehsvh","mlfg":"y"," uxni":"bh","rxinpnts":"u"..  9%   {"
      cepy":"vvryehsvh","mlfg":"y"," uxni":"bh","rxinpnts":"u".. 12%   {"
      cepy":"vvryehsvh","mlfg":"y"," uxni":"bh","rxinpnts":"u".. 13%   {"
      cepy":"vvryehsvh","mlfg":"y"," uxni":"bh","rxinpnts":"u".. 14%   {"
      1000x fleece.json(t, "E4")        200ns/element  34%,   7%  17%   {"\rcepy":"\u007fvvryehsvh","mlfg":"y","\tuxni":"bh","rxinpn.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 30
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        360ns/element                 ["rhc\\","nbl\fphgzu","cwwgevxu","ve","meunz","vlqd\bxr","mv.. 
      1000x json4.encode(t)            2120ns/element                 ["rhc\\","nbl
                                                                                   phgzu","cwwgevxu","ve","meunz","vlqxr","mv",.. 
      1000x yajl.to_string(t)           620ns/element                 ["rhc\\","nbl\fphgzu","cwwgevxu","ve","meunz","vlqd\bxr","mv.. 
      1000x fleece.json(t, "E0")         60ns/element  16%, 2.8%   9%   ["rhc\","nbl
                                                                                    phgzu","cwwgevxu","ve","meunz","vlqxr","mv",".. 
      1000x fleece.json(t, "E1")         80ns/element  22%,   3%  12%   ["rhc*","nbl
                                                                                    phgzu","cwwgevxu","ve","meunz","vlqxr","mv",".. 
      1000x fleece.json(t, "E2")         80ns/element  22%,   3%  12%   ["rhc\\","nbl
                                                                                     phgzu","cwwgevxu","ve","meunz","vlqxr","mv",.. 
      1000x fleece.json(t, "E3")         80ns/element  22%,   3%  12%   ["rhc\\","nbl
                                                                                     phgzu","cwwgevxu","ve","meunz","vlqxr","mv",.. 
      1000x fleece.json(t, "E4")         90ns/element  25%,   4%  14%   ["rhc\\","nbl\fphgzu","cwwgevxu","ve","meunz","vlqd\bxr","mv.. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        580ns/element                 {"efxzpcq":"xtkbeaueq","hoyuxznmzy":"hile","pvmdmum":"alnzkp.. 
      1000x json4.encode(t)            2890ns/element                 {"efxzpcq":"xtkbeaueq","hoyuxznmzy":"hile","pvmdmum":"alnzkp.. 
      1000x yajl.to_string(t)          1100ns/element                 {"efxzpcq":"xtkbeaueq","hoyuxznmzy":"hile","pvmdmum":"alnzkp.. 
      1000x fleece.json(t, "E0")        110ns/element  18%,   3%   9%   {"lmaxnisk":"hoqwyb","
                                                                                               pxpdbavz":"koaxaj","qdbgll":"oyerkd.. 
      1000x fleece.json(t, "E1")        140ns/element  24%,   4%  12%   {"lmaxnisk":"hoqwyb","
                                                                                               pxpdbavz":"koaxaj","qdbgll":"oyerkd.. 
      1000x fleece.json(t, "E2")        150ns/element  25%,   5%  13%   {"lmaxnisk":"hoqwyb","
                                                                                               pxpdbavz":"koaxaj","qdbgll":"oyerkd.. 
      1000x fleece.json(t, "E3")        150ns/element  25%,   5%  13%   {"lmaxnisk":"hoqwyb","
                                                                                               pxpdbavz":"koaxaj","qdbgll":"oyerkd.. 
      1000x fleece.json(t, "E4")        170ns/element  29%,   5%  15%   {"lmaxnis\u0001k":"hoqwyb","\fpxpdbavz":"koaxaj\u007f","qdbg.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 100
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        350ns/element                 ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x json4.encode(t)            2180ns/element                 ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x yajl.to_string(t)           610ns/element                 ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x fleece.json(t, "E0")         60ns/element  17%, 2.7%   9%   ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x fleece.json(t, "E1")         80ns/element  22%,   3%  13%   ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x fleece.json(t, "E2")         80ns/element  22%,   3%  13%   ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x fleece.json(t, "E3")         80ns/element  22%,   3%  13%   ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      1000x fleece.json(t, "E4")         80ns/element  22%,   3%  13%   ["h","hkugb","cfjalbjv","dknj","qgm","rizs","cpz","ulapllfw".. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        550ns/element                 {"sinlryafu":"vrd\th","xxtk":"elysfrh","okigdxc":"yqavud","m.. 
      1000x json4.encode(t)            2910ns/element                 {"sinlryafu":"vrd\th","xxtk":"elysfrh","okigdxc":"yqavud","m.. 
      1000x yajl.to_string(t)          1050ns/element                 {"sinlryafu":"vrd\th","xxtk":"elysfrh","okigdxc":"yqavud","m.. 
      1000x fleece.json(t, "E0")        110ns/element  20%,   3%  10%   {"hgikjcob":"borj","wyi":"vew","vbxkrwyuwr":"duxgsf","livxfs.. 
      1000x fleece.json(t, "E1")        140ns/element  25%,   4%  13%   {"hgikjcob":"borj","wyi":"vew","vbxkrwyuwr":"duxgsf","livxfs.. 
      1000x fleece.json(t, "E2")        160ns/element  29%,   5%  15%   {"hgikjcob":"borj","wyi":"vew","vbxkrwyuwr":"duxgsf","livxfs.. 
      1000x fleece.json(t, "E3")        150ns/element  27%,   5%  14%   {"hgikjcob":"borj","wyi":"vew","vbxkrwyuwr":"duxgsf","livxfs.. 
      1000x fleece.json(t, "E4")        160ns/element  29%,   5%  15%   {"hgikjcob":"borj","wyi":"vew","vbxkrwyuwr":"duxgsf","livxfs.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 1000
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        350ns/element                 ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x json4.encode(t)            2220ns/element                 ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x yajl.to_string(t)           600ns/element                 ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x fleece.json(t, "E0")         70ns/element  20%,   3%  11%   ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x fleece.json(t, "E1")         70ns/element  20%,   3%  11%   ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x fleece.json(t, "E2")         80ns/element  22%,   3%  13%   ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x fleece.json(t, "E3")         80ns/element  22%,   3%  13%   ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      1000x fleece.json(t, "E4")         80ns/element  22%,   3%  13%   ["zsvn","ke","pcruqjlbr","k","mewigqwufj","uzbsyx","bbyhq",".. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        560ns/element                 {"htbxjnlik":"ihgrhwaf","mgpy":"m","mugrduq":"emjnc","gkdyx".. 
      1000x json4.encode(t)            2840ns/element                 {"htbxjnlik":"ihgrhwaf","mgpy":"m","mugrduq":"emjnc","gkdyx".. 
      1000x yajl.to_string(t)          1050ns/element                 {"htbxjnlik":"ihgrhwaf","mgpy":"m","mugrduq":"emjnc","gkdyx".. 
      1000x fleece.json(t, "E0")        110ns/element  19%,   3%  10%   {"jcacsn":"gxm","owqr":"rh","ozrwmyalma":"xsdpvcqq","lefxp":.. 
      1000x fleece.json(t, "E1")        130ns/element  23%,   4%  12%   {"jcacsn":"gxm","owqr":"rh","ozrwmyalma":"xsdpvcqq","lefxp":.. 
      1000x fleece.json(t, "E2")        150ns/element  26%,   5%  14%   {"jcacsn":"gxm","owqr":"rh","ozrwmyalma":"xsdpvcqq","lefxp":.. 
      1000x fleece.json(t, "E3")        150ns/element  26%,   5%  14%   {"jcacsn":"gxm","owqr":"rh","ozrwmyalma":"xsdpvcqq","lefxp":.. 
      1000x fleece.json(t, "E4")        130ns/element  23%,   4%  12%   {"jcacsn":"gxm","owqr":"rh","ozrwmyalma":"xsdpvcqq","lefxp":.. 
      ---------------------------------------------------------------------------------
      Note that fleece may list associative arrays in different order.
      Also note that output can be derailed by random ctrl characters.
      Also see test/README.md
      
## Fleece (with inline assembler) vs JSON4, LuaJSON C lib and Lua-Yajl

This is slightly faster, around 10-20% actually. Inline assembler is available for x86 to speed up the escaping functionality.      
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=i                   
      1000x luajson.stringify(t)        680ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x json4.encode(t)            1930ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x yajl.to_string(t)          1200ns/element                 [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E0")         30ns/element   4%, 1.5% 2.4%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E1")         20ns/element 2.9%,   1% 1.6%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E2")         40ns/element   5%,   2%   3%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E3")         30ns/element   4%, 1.5% 2.5%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      1000x fleece.json(t, "E4")         30ns/element   4%, 1.5% 2.4%   [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 10
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        390ns/element                 ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      1000x json4.encode(t)            2180ns/element                 ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      1000x yajl.to_string(t)           590ns/element                 ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      1000x fleece.json(t, "E0")         60ns/element  15%, 2.7%  10%   ["e","pydrv","gzxcw",""junakl","ctjxxz","w","weeslvh","kce<".. 
      1000x fleece.json(t, "E1")         70ns/element  17%,   3%  11%   ["e","pydrv","gzxcw","*junakl","ctjxxz","w","weeslvh","kce<".. 
      1000x fleece.json(t, "E2")         80ns/element  20%,   3%  13%   ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      1000x fleece.json(t, "E3")         70ns/element  17%,   3%  11%   ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      1000x fleece.json(t, "E4")         80ns/element  20%,   3%  13%   ["e","pydrv","gzxcw","\"junakl","ctjxxz","w","weeslvh","kce<.. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        580ns/element                 {"\rplselncv":"nwfkdjt","gtwuqmqzv":"ok","a\"lwomg\nqk":"mb".. 
      plselncv":"nwfkdjt","gtwuqmqzv":"ok","a\"lwomg\nqk":"mb",..     {"
      1000x yajl.to_string(t)          1060ns/element                 {"\rplselncv":"nwfkdjt","gtwuqmqzv":"ok","a\"lwomg\nqk":"mb".. 
      1000x fleece.json(t, "E0")        110ns/element  18%,   3%  10%   {"qlqh
      ivcx":"rx
         m","kt":"	g
      gwyxohu","zqtokkvz":""ddc.. 
      1000x fleece.json(t, "E1")        120ns/element  20%,   4%  11%   {"qlqh
      ivcx":"rx
         m","kt":"	g
      gwyxohu","zqtokkvz":"*ddc.. 
      1000x fleece.json(t, "E2")        140ns/element  24%,   5%  13%   {"qlqh
      ivcx":"rx
         m","kt":"	g
      gwyxohu","zqtokkvz":"\"dd.. 
      1000x fleece.json(t, "E3")        130ns/element  22%,   4%  12%   {"qlqh
      ivcx":"rx
         m","kt":"	g
      gwyxohu","zqtokkvz":"\"dd.. 
      1000x fleece.json(t, "E4")        150ns/element  25%,   5%  14%   {"qlqh\nwhxx":"rx\rivc\fm","kt":"\tg\ngwyxohu","zqtoka\bkvz".. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 30
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        390ns/element                 ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x json4.encode(t)            2180ns/element                 ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x yajl.to_string(t)           570ns/element                 ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x fleece.json(t, "E0")         60ns/element  15%, 2.7%  10%   ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x fleece.json(t, "E1")         60ns/element  15%, 2.7%  10%   ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x fleece.json(t, "E2")         70ns/element  17%,   3%  12%   ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x fleece.json(t, "E3")         80ns/element  20%,   3%  14%   ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      1000x fleece.json(t, "E4")         70ns/element  17%,   3%  12%   ["mfsb","wdpue","ntazf","shs<","ub","fmrwa","lxrk","ghlxgfp".. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        560ns/element                 {"kpta":"xryllakhz","jxbd":"zhhsrkdyip","tovlmjgeg":"qzkgh",.. 
      1000x json4.encode(t)            2880ns/element                 {"kpta":"xryllakhz","jxbd":"zhhsrkdyip","tovlmjgeg":"qzkgh",.. 
      1000x yajl.to_string(t)          1040ns/element                 {"kpta":"xryllakhz","jxbd":"zhhsrkdyip","tovlmjgeg":"qzkgh",.. 
      1000x fleece.json(t, "E0")        110ns/element  19%,   3%  10%   {"bueoy":"jebqh","btkf":"yenepgppi","hryjm":"ncvwx","czxin":.. 
      1000x fleece.json(t, "E1")        130ns/element  23%,   4%  12%   {"bueoy":"jebqh","btkf":"yenepgppi","hryjm":"ncvwx","czxin":.. 
      1000x fleece.json(t, "E2")        130ns/element  23%,   4%  12%   {"bueoy":"jebqh","btkf":"yenepgppi","hryjm":"ncvwx","czxin":.. 
      1000x fleece.json(t, "E3")        140ns/element  25%,   4%  13%   {"bueoy":"jebqh","btkf":"yenepgppi","hryjm":"ncvwx","czxin":.. 
      1000x fleece.json(t, "E4")        130ns/element  23%,   4%  12%   {"bueoy":"jebqh","btkf":"yenepgppi","hryjm":"ncvwx","czxin":.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 100
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        370ns/element                 ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x json4.encode(t)            2200ns/element                 ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x yajl.to_string(t)           570ns/element                 ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x fleece.json(t, "E0")         70ns/element  18%,   3%  12%   ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x fleece.json(t, "E1")         70ns/element  18%,   3%  12%   ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x fleece.json(t, "E2")         70ns/element  18%,   3%  12%   ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x fleece.json(t, "E3")         70ns/element  18%,   3%  12%   ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      1000x fleece.json(t, "E4")         70ns/element  18%,   3%  12%   ["iimtttkqwk","crvicl","otwe","oehmce","zhzofclm","azhidje",.. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        550ns/element                 {"brqley":"firnu","xeritto":"s","dtq":"fpxdov","slf":"rscsfe.. 
      1000x json4.encode(t)            2890ns/element                 {"brqley":"firnu","xeritto":"s","dtq":"fpxdov","slf":"rscsfe.. 
      1000x yajl.to_string(t)          1020ns/element                 {"brqley":"firnu","xeritto":"s","dtq":"fpxdov","slf":"rscsfe.. 
      1000x fleece.json(t, "E0")        120ns/element  21%,   4%  11%   {"dagtqqif":"ipyzjeoyhp","wzwln":"oqss","qclbehl":"nesxf","w.. 
      1000x fleece.json(t, "E1")        120ns/element  21%,   4%  11%   {"dagtqqif":"ipyzjeoyhp","wzwln":"oqss","qclbehl":"nesxf","w.. 
      1000x fleece.json(t, "E2")        130ns/element  23%,   4%  12%   {"dagtqqif":"ipyzjeoyhp","wzwln":"oqss","qclbehl":"nesxf","w.. 
      1000x fleece.json(t, "E3")        130ns/element  23%,   4%  12%   {"dagtqqif":"ipyzjeoyhp","wzwln":"oqss","qclbehl":"nesxf","w.. 
      1000x fleece.json(t, "E4")        130ns/element  23%,   4%  12%   {"dagtqqif":"ipyzjeoyhp","wzwln":"oqss","qclbehl":"nesxf","w.. 
      ---------------------------------------------------------------------------------
      Frequency of escape characters 1 in 1000
      ---------------------------------------------------------------------------------
      1000 elements in t[i]=randstr(i)          
      1000x luajson.stringify(t)        380ns/element                 ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x json4.encode(t)            2190ns/element                 ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x yajl.to_string(t)           560ns/element                 ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x fleece.json(t, "E0")         60ns/element  15%, 2.7%  10%   ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x fleece.json(t, "E1")         70ns/element  18%,   3%  12%   ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x fleece.json(t, "E2")         70ns/element  18%,   3%  12%   ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x fleece.json(t, "E3")         70ns/element  18%,   3%  12%   ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      1000x fleece.json(t, "E4")         70ns/element  18%,   3%  12%   ["vstkztoi","oenamq","vwhotsfcu","eti","uwkgfwfi","nfzexxqay.. 
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
      1000x luajson.stringify(t)        550ns/element                 {"tekcl":"qtuq","yb":"mxsbffksr","vfvmrfl":"ygowwrd","ugah":.. 
      1000x json4.encode(t)            2860ns/element                 {"tekcl":"qtuq","yb":"mxsbffksr","vfvmrfl":"ygowwrd","ugah":.. 
      1000x yajl.to_string(t)          1010ns/element                 {"tekcl":"qtuq","yb":"mxsbffksr","vfvmrfl":"ygowwrd","ugah":.. 
      1000x fleece.json(t, "E0")        120ns/element  21%,   4%  11%   {"spphgji":"ovygm","pjpuyeeof":"xszvpo","lpqzujwohr":"ol","w.. 
      1000x fleece.json(t, "E1")        120ns/element  21%,   4%  11%   {"spphgji":"ovygm","pjpuyeeof":"xszvpo","lpqzujwohr":"ol","w.. 
      1000x fleece.json(t, "E2")        130ns/element  23%,   4%  12%   {"spphgji":"ovygm","pjpuyeeof":"xszvpo","lpqzujwohr":"ol","w.. 
      1000x fleece.json(t, "E3")        130ns/element  23%,   4%  12%   {"spphgji":"ovygm","pjpuyeeof":"xszvpo","lpqzujwohr":"ol","w.. 
      1000x fleece.json(t, "E4")        130ns/element  23%,   4%  12%   {"spphgji":"ovygm","pjpuyeeof":"xszvpo","lpqzujwohr":"ol","w.. 
      ---------------------------------------------------------------------------------
      Note that fleece may list associative arrays in different order.
      Also note that output can be derailed by random ctrl characters.
      Also see test/README.md

Please make your benchmarks and contribute them, thank you.