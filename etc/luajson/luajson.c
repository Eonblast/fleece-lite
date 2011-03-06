#if defined __WIN32__ || defined WIN32
# include <windows.h>
# define _EXPORT __declspec(dllexport)
#else
# define _EXPORT
#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

static const char ESC=27;
static const char OPN=28;
static const char CLS=29;

/* hd */ /* added to make it compile on Mac OS X */
/* hd */ #define BOOL int             
/* hd */ #define TRUE 1               
/* hd */ #define FALSE 0              

//--- auxliary functions -------------------------------------------
/*
static void stackDump (lua_State *L)
{
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++)
    {  // repeat for each level
        int t = lua_type(L, i);
        switch (t) {
        case LUA_TSTRING:  // strings
            printf("`%s'", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:  // booleans
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:  // numbers
            printf("%g", lua_tonumber(L, i));
            break;
        default:  // other values
            printf("%s", lua_typename(L, t));
            break;
        }
        printf("  ");  // put a separator
    }
    printf("\n");  // end the listing
}
//*/

void JSONError(lua_State *L, const char* msg)
{
    // what
    lua_pushlstring(L, msg, strlen(msg));
    lua_error(L);
}

void JSONDebug(lua_State *L, const char* msg)
{
    lua_getglobal(L, "print");
    lua_pushlstring(L, msg, strlen(msg));
    lua_call(L, 1, 0);
}

unsigned int digit2int(lua_State *L, const unsigned char digit)
{
    unsigned int val;
    if (digit >= '0' && digit <= '9')
        val = digit - '0';
    else if (digit >= 'a' || digit <= 'f')
        val = digit - 'a' + 10;
    else if (digit >= 'A' || digit <= 'F')
        val = digit - 'A' + 10;
    else
        JSONError(L, "Invalid hex digit");
    return val;
}

unsigned int fourhex2int(lua_State *L, const unsigned char * code)
{
    unsigned int utf = 0;
    utf += digit2int(L, code[0]) * 4096;
    utf += digit2int(L, code[1]) * 256;
    utf += digit2int(L, code[2]) * 16;
    utf += digit2int(L, code[3]);
    return utf;
}

static const char* code2utf8(lua_State *L, const unsigned char * code, char buf[4])
{
    unsigned int utf = 0;
    utf = fourhex2int(L, code);
    if (utf < 128)
    {
        buf[0] = utf & 0x7F;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        return buf;
    }
    else if (utf < 2048)
    {
        buf[0] = ((utf >> 6) & 0x1F) | 0xC0;
        buf[1] = (utf & 0x3F) | 0x80;
        buf[2] = 0;
        buf[3] = 0;
        return buf;
    }
    else
    {
        buf[0] = ((utf >> 12) & 0x0F) | 0xE0;
        buf[1] = ((utf >> 6) & 0x3F) | 0x80;
        buf[2] = (utf & 0x3F) | 0x80;
        buf[3] = 0;
        return buf;
    }
    return buf;
}

void passwhitespace(lua_State *L, char ** c)
{
    while (**c == ' ' || **c == '\r' || **c == '\n' || **c == '\t')
    {
        (*c)++;
    }
}

void readValue(lua_State *L, char * str, char ** c);
void readString(lua_State *L, char * str, char ** c);

void readArray(lua_State *L, char * str, char ** c)
{
    if (**c != '[')
        JSONError(L, "JSON readArray (1) b0rked :c.");
    //JSONError(L, strcat(*c, "JSON readArray b0rked :c."));
    (*c)++;
    int i = 1;
    lua_newtable(L);
    for (i = 1; 1; i++)
    {
        passwhitespace(L, c);
        lua_pushinteger(L, i);
        //JSONError(L, ": hmm...");
        readValue(L, str, c);
        lua_settable(L, -3);
        passwhitespace(L, c);
        if (**c == ',')
        {
            (*c)++;
            passwhitespace(L, c);
        }
        else
            break;
    }
    passwhitespace(L, c);
    if (**c == ']')
        (*c)++;
    else
        JSONError(L, strcat(*c, "JSON readArray b0rked :c."));
}

void readFalse(lua_State *L, char * str, char ** c)
{
    if (!strncmp(*c, "false", 5))
    {
        lua_pushboolean(L, 0);
        *c += 5;
    }
    else
        JSONError(L, "JSON readFalse b0rked :c.");
}

void readNull(lua_State *L, char * str, char ** c)
{
    if (!strncmp(*c, "null", 4))
    {
        lua_getglobal(L, "json");
        lua_getfield(L, -1, "null");
        lua_remove(L, -2);
        *c += 4;
    }
    else
        JSONError(L, "JSON readNull b0rked :c.");
}

void readNumber(lua_State *L, char * str, char ** c)
{
    float neg;
    if (**c == '-')
    {
        neg = -1;
        (*c)++;
    }
    else
        neg = 1;
    double num = 0;
    char * save;
    save = *c;
    char * foundDec = NULL, * foundExp = NULL, * foundEnd = NULL;
    while (foundEnd == NULL)
    {
        switch (**c)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;
        case '.':
            if (foundDec != NULL || foundExp != NULL)
                JSONError(L, "bad number format");
            foundDec = *c;
            break;
        case 'e':
        case 'E':
            if (foundExp != NULL)
                JSONError(L, "bad number format");
            foundExp = *c;
            break;
        case '-':
        case '+':
            //if (*((*c)-1) != 'e' || *((*c)-1) != 'E')
                //JSONError(L, "bad number format");
            break;
        default:
            foundEnd = *c;
            break;
        }
        (*c)++;
    }
    *c = save;
    int exp;
    double exponent = 0;
    int exponentneg = 1;
    if (foundDec != NULL)
    {
        exp = foundDec - *c - 1;
        for (; **c != '.'; exp--)
        {
            num += (**c - '0') * pow(10, exp);
            (*c)++;
        }
        (*c)++;
        if (foundExp != NULL)
        {
            for (; **c != 'e' && **c != 'E'; exp--)
            {
                num += (**c - '0') * pow(10, exp);
                (*c)++;
            }
            (*c)++;
            if (**c == '-')
            {
                exponentneg = -1;
                (*c)++;
            }
            else if (**c == '+')
            {
                (*c)++;
            }
            exp = foundEnd - *c - 1;
            for (; *c != foundEnd; exp--)
            {
                exponent += (**c - '0') * pow(10, exp);
                (*c)++;
            }
        }
        else
        {
            for (; *c != foundEnd; exp--)
            {
                num += (**c - '0') * pow(10, exp);
                (*c)++;
            }
        }
    }
    else if (foundExp != NULL)
    {
        exp = foundExp - *c - 1;
        for (; **c != 'e' && **c != 'E'; exp--)
        {
            num += (**c - '0') * pow(10, exp);
            (*c)++;
        }
        (*c)++;
        if (**c == '-')
        {
            exponentneg = -1;
            (*c)++;
        }
        else if (**c == '+')
        {
            (*c)++;
        }
        exp = foundEnd - *c - 1;
        for (; *c != foundEnd; exp--)
        {
            exponent += (**c - '0') * pow(10, exp);
            (*c)++;
        }
    }
    else
    {
        exp = foundEnd - *c - 1;
        for (; exp >= 0; exp--)
        {
            num += (**c - '0') * pow(10, exp);
            (*c)++;
        }
    }
    num = neg * num * pow(10, exponentneg * exponent);
    lua_pushnumber(L, num);
}

void readObject(lua_State *L, char * str, char ** c)
{
    if (**c != '{')
        JSONError(L, "JSON readObject(1) b0rked :c.");
    (*c)++;
    lua_newtable(L);
    while (1)
    {
        passwhitespace(L, c);
        readString(L, str, c);
        passwhitespace(L, c);
        if (**c != ':')
        {
            JSONError(L, "JSON readObject(2) b0rked :c.");
        }
        (*c)++;
        passwhitespace(L, c);
        readValue(L, str, c);
        lua_settable(L, -3);
        passwhitespace(L, c);
        if (**c == ',')
        {
            (*c)++;
            passwhitespace(L, c);
        }
        else
            break;
    }
    passwhitespace(L, c);
    if (**c == '}')
        (*c)++;
    else
        JSONError(L, strcat(*c, "JSON readObject(3) b0rked :c."));
}

void readString(lua_State *L, char * str, char ** c)
{
    if (**c != '"')
        JSONError(L, "no string :c");
    (*c)++;
    int minus = 0;
    char * beginning = *c;
    char * end = NULL;
    unsigned int utf;
    while (end == NULL)
    {
        switch (**c)
        {
        case '"':
            if (*((*c) - 1) != '\\')
                end = *c;
            break;
        case '\\':
            switch (*((*c) + 1))
            {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                minus++;
                (*c)++;
                break;
            case 'u':
                /* hd: fix warning */utf = fourhex2int(L, (unsigned char *)((*c) + 2));
                if (utf < 128)
                    minus += 5;
                else if (utf < 2048)
                    minus += 4;
                else
                    minus += 3;
                (*c) += 5;
                break;
            default:
                JSONError(L, "invalid escape character");
                break;
            }
        default:
            break;
        }
        (*c)++;
    }
    *c = beginning;
    char * newstr = NULL;
    size_t len = sizeof(char) * (end - beginning - minus);
    newstr = malloc(len+1);
    memset(newstr, 0, len+1);
    char * newc = newstr;
    char * nextEscape = NULL;
    char utfbuf[4] = "";
    while (*c != end)
    {
        nextEscape = strchr(*c, '\\');
        if (nextEscape > end)
            nextEscape = NULL;
        if (nextEscape == *c)
        {
            switch (*((*c) + 1))
            {
            case '"':
                *newc = '"';
                newc++;
                (*c) += 2;
                break;
            case '\\':
                *newc = '\\';
                newc++;
                (*c) += 2;
                break;
            case '/':
                *newc = '/';
                newc++;
                (*c) += 2;
                break;
            case 'b':
                *newc = '\b';
                newc++;
                (*c) += 2;
                break;
            case 'f':
                *newc = '\f';
                newc++;
                (*c) += 2;
                break;
            case 'n':
                *newc = '\n';
                newc++;
                (*c) += 2;
                break;
            case 'r':
                *newc = '\r';
                newc++;
                (*c) += 2;
                break;
            case 't':
                *newc = '\t';
                newc++;
                (*c) += 2;
                break;
            case 'u':
                /* hd fix warning */code2utf8(L, (unsigned char *)((*c) + 2), utfbuf);
                size_t len = strlen(utfbuf);
                strcpy(newc, utfbuf);
                newc += len;
                (*c) += 6;
                break;
            default:
                JSONError(L, "invalid escape character");
                break;
            }
        }
        else if (nextEscape != NULL)
        {
            size_t len = nextEscape - *c;
            strncpy(newc, *c, len);
            newc += len;
            (*c) += len;
        }
        else
        {
            size_t len = end - *c;
            strncpy(newc, *c, len);
            newc += len;
            (*c) += len;
        }
    }
    *newc = 0;
    lua_pushlstring(L, newstr, strlen(newstr));
    (*c)++;
    free(newstr);
}

void readTrue(lua_State *L, char * str, char ** c)
{
    if (!strncmp(*c, "true", 4))
    {
        lua_pushboolean(L, 1);
        *c += 4;
    }
    else
        JSONError(L, "JSON readTrue b0rked :c.");
}

void readValue(lua_State *L, char * str, char ** c)
{
    switch (**c)
    {
    case 'f':
        readFalse(L, str, c);
        break;
    case 'n':
        readNull(L, str, c);
        break;
    case 't':
        readTrue(L, str, c);
        break;
    case '[':
        readArray(L, str, c);
        break;
    case '{':
        readObject(L, str, c);
        break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        readNumber(L, str, c);
        break;
    case '"':
        readString(L, str, c);
        break;
    default:
        JSONError(L, strcat(*c, "JSON readValue b0rked :c."));
        lua_error(L);
        break;
    }
}

int JSON_parse(lua_State *L)
{
    char * str = NULL;
    size_t str_size = 0;
    if (lua_isuserdata(L, 1))
        str = (char *)lua_touserdata(L, 1);
    else
    {
        const char * sTmp = luaL_checklstring(L, 1, &str_size);
        str = (char *)malloc(str_size + 1);
        memcpy(str, sTmp, str_size);
        str[str_size] = 0;
    }

    char * c = str;
    passwhitespace(L, &c);
    readValue(L, str, &c);
    passwhitespace(L, &c);
    if (*c != 0)
        JSONError(L, "Something past the root value?");
     free(str);
    return 1;
}

// ~~~ STRINGIFY ~~~

char int2digit(const int val)
{
    if (val >= 10)
        return 'a' + val - 10;
    else
        return '0' + val;
}

void int2fourhex(int num, char * buf)
{
    buf[0] = int2digit(num / 4096);
    num %= 4096;
    buf[1] = int2digit(num / 256);
    num %= 256;
    buf[2] = int2digit(num / 16);
    num %= 16;
    buf[3] = int2digit(num);
    buf[4] = 0;
}

char * quote(const char * S, int len)
{
    const char * c = S;
    int count = 2 + len;    // final string size, excluding zero terminator
    while (c < S + len)
    {
        switch (*c)
        {
        case '\\':
        case '"':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
            count++;
            break;
        default:
            if (*c < 32)
                count += 5;
            break;
        }
        c++;
    }
    // count complete, allocate and recreate string.
    char * newS = malloc(sizeof(char) * (count + 1)); // add null terminator
    char * newc = newS;
    newc[0] = '"';
    newc[1] = 'a';
    newc[2] = 'b';
    newc[3] = 'c';
    newc++;
    c = S;
    while (c < S + len)
    {
        switch (*c)
        {
        case '\\':
            newc[0] = '\\';
            newc[1] = '\\';
            newc += 2;
            break;
        case '"':
            newc[0] = '\\';
            newc[1] = '"';
            newc += 2;
            break;
        case '\b':
            newc[0] = '\\';
            newc[1] = 'b';
            newc += 2;
            break;
        case '\f':
            newc[0] = '\\';
            newc[1] = 'f';
            newc += 2;
            break;
        case '\n':
            newc[0] = '\\';
            newc[1] = 'n';
            newc += 2;
            break;
        case '\r':
            newc[0] = '\\';
            newc[1] = 'r';
            newc += 2;
            break;
        case '\t':
            newc[0] = '\\';
            newc[1] = 't';
            newc += 2;
            break;
        default:
            if (*c < 32)
            {
                newc[0] = '\\';
                newc[1] = 'u';
                newc += 2;
                int2fourhex(*c, newc);
                newc += 4;
            }
            else
            {
                newc[0] = *c;
                newc++;
            }
            break;
        }
        c++;
    }
    newc[0] = '"';
    newc[1] = 0;
    return newS;
}

typedef struct {void * value; struct s_linkedList * next;} linkedList;

void freeList(linkedList *L)
{
    if (L == NULL)
        return;
    linkedList * ptr = (linkedList *)L->next;
    free(L);
    freeList(ptr);
}

void addToList(void * Value, linkedList * L)
{
    if (L == NULL)
    {
        L = malloc(sizeof(linkedList));
        L->value = Value;
        L->next = NULL;
    }
    else
    {
        if (L->next != NULL)
        {
            addToList(Value, (linkedList *)L->next);
        }
        else
        {
            linkedList * newL = (linkedList *)malloc(sizeof(linkedList));
            L->next = (struct s_linkedList *)newL;
            newL->value = Value;
            newL->next = NULL;
        }
    }
}

linkedList * findInList(linkedList * L, void * Match)
{
    linkedList * ptr;
    linkedList * match = NULL;
    for (ptr = L; ptr != NULL; ptr = (linkedList *)ptr->next)
    {
        if (ptr->value == Match)
        {
            match = ptr;
            break;
        }
    }
    return match;
}

void removeLastFromList(linkedList * L, linkedList * Last)
{
    if (L == NULL)
    {
        return;
    }
    if (L->next == NULL)
    {
        free(L);
        if (Last != NULL)
        {
            Last->next = NULL;
        }
    }
    else
        removeLastFromList((linkedList *)L->next, L);
}

linkedList * baseList;

/// -0 +0
void stringifyBoolean(lua_State *L, const int value, luaL_Buffer *StringBuf)
{
    luaL_addstring(StringBuf, value?"true":"false");
}

/// -0 +0
void stringifyNull(lua_State *L, luaL_Buffer *StringBuf)
{
    luaL_addstring(StringBuf, "null");
}

/// -0 +0
void stringifyNumber(lua_State *L, const double value, luaL_Buffer *StringBuf)
{
    char s[32];
    sprintf(s, "%.14g", value);
    luaL_addstring(StringBuf, s);
}

/// -0 +0
void stringifyString(lua_State *L, const char * S, const int len, luaL_Buffer *StringBuf)
{
    char * quote_s = quote(S, len);
    luaL_addstring(StringBuf, quote_s);
    free(quote_s);
}

void stringify(lua_State *L, luaL_Buffer *StringBuf);

/// Takes a table at the top of the stack, and appends it, stringified, to StringBuf.
void stringifyTable(lua_State *L, luaL_Buffer *StringBuf)
{
    void * tablePtr = (void *)lua_topointer(L, -1); // -0 +0
    linkedList * foundMatch = findInList(baseList, tablePtr);
    if (!foundMatch)
    {
        addToList(tablePtr, baseList);
        lua_checkstack(L, 6);
        lua_pushnumber(L, 1); // -0 +1
        lua_gettable(L, -2);  // -1 +1
        if (lua_isnil(L, -1))   // No t[1], treat as object.
        {
            lua_pop(L, 1);  // -1 +0
            luaL_addchar(StringBuf, '{');
            lua_getglobal(L, "pairs"); // -0 +1
            lua_pushvalue(L, -2);   // -0 +1
            lua_call(L, 1, 3);  // -2 +3 pairs(t), three return-values put on stack
            BOOL first = TRUE;
            while (1)
            {
                lua_pushvalue(L, -3);   // -0 +1
                lua_pushvalue(L, -3);   // -0 +1
                lua_pushvalue(L, -3);   // -0 +1
                lua_remove(L, -4);   // -1 +0
                lua_call(L, 2, 2);  // -3 +2 calling the iterator function, getting key,value
                if (lua_isnil(L, -2))
                {
                    lua_pop(L, 4); // -4+0
                    break;
                }
                else if (!lua_isstring(L, -2))
                {
                    lua_pop(L, 1); // -1+0
                    continue;
                }
                if (!first)
                    luaL_addchar(StringBuf, ',');
                else
                    first = FALSE;
                lua_pushvalue(L, -2);   // -0 +1
                stringify(L, StringBuf); // -1 +0
                luaL_addchar(StringBuf, ':');   // -0 +0
                stringify(L, StringBuf); // -1 +0
            }
            luaL_addchar(StringBuf, '}');
        }
        else    // t[1] exists, treat as array.
        {
            lua_pop(L, 1);  // -1 +0
            luaL_addchar(StringBuf, '[');
            lua_getglobal(L, "ipairs"); // -0+1
            lua_pushvalue(L, -2);   // -0+1
            lua_call(L, 1, 3);  // -2+3 ipairs(t), three return-values put on stack
            BOOL first = TRUE;
            while (1)
            {
                lua_pushvalue(L, -3);   // -0+1
                lua_pushvalue(L, -3);   // -0+1
                lua_pushvalue(L, -3);   // -0+1
                lua_remove(L, -4);   // -1+0
                lua_call(L, 2, 2);  // -3+2 calling the iterator function, getting key,value
                if (lua_isnil(L, -2))
                {
                    lua_pop(L, 4); // -4+0
                    break;
                }
                if (!first)
                    luaL_addchar(StringBuf, ',');
                else
                    first = FALSE;
                stringify(L, StringBuf); // -1 +0
            }
            luaL_addchar(StringBuf, ']');
        }
        removeLastFromList(baseList, NULL);
    }
    else
    {
        printf("RECURSION\n");
        luaL_addstring(StringBuf, "RECURSION");
    }
}

/// Takes the value at the top of L's stack, and appends it to StringBuf.
/// -1 +0
void stringify(lua_State *L, luaL_Buffer *StringBuf)
{
    if (lua_isboolean(L, -1))
    {
        int B = lua_toboolean(L, -1);   // -0 +0
        stringifyBoolean(L, B, StringBuf);  // -0 +0
    }
    else if (lua_isnumber(L, -1))
    {
        double N = lua_tonumber(L, -1); // -0 +0
        stringifyNumber(L, N, StringBuf);   // -0 +0
    }
    else if (lua_isstring(L, -1))
    {
        /* hd: fix warning */ size_t len;
        const char * s = lua_tolstring(L, -1, &len);    // -0 +0
        stringifyString(L, s, len, StringBuf);  // -0 +0
    }
    else if (lua_istable(L, -1))
    {
        lua_checkstack(L, 2);
        lua_getglobal(L, "json");   // -0 +1
        lua_getfield(L, -1, "null");    // -0 +1
        if (lua_equal(L, -3, -1))
        {
            lua_pop(L, 2);  // -2 +0
            stringifyNull(L, StringBuf);    // -0 +0
        }
        else
        {
            lua_pop(L, 2);  // -2 +0
            stringifyTable(L, StringBuf);   // -0 +0
        }
    }
    else
    {
        if (lua_isfunction(L, -1))
            luaL_addstring(StringBuf, "FUNCTION");
        else if (lua_isuserdata(L, -1))
            luaL_addstring(StringBuf, "USERDATA");
        else if (lua_isnil(L, -1))
            luaL_addstring(StringBuf, "");
        else
            luaL_addstring(StringBuf, "wtf?");
    }
    lua_pop(L, 1); // -1 +0
}

int JSON_stringify(lua_State *L)
{
    lua_State * Strings = lua_open(); // Lua state which will hold the string buffer stack.
    luaL_Buffer * LBuf = malloc(sizeof(luaL_Buffer));
    luaL_buffinit(Strings, LBuf); // This one.
    lua_settop(L, 1);   // Only take the first argument.
    stringify(L, LBuf); // -1 +0
    luaL_pushresult(LBuf);  // -0 +1
    free(LBuf);
    lua_xmove(Strings, L, 1);   // Move the finished string to the main state.
    /* hd: causes seg faults. Accepting probably leak instead taking out: lua_close(Strings); */
    return 1;
}

int _EXPORT luaopen_luajson (lua_State* L)
{
    static const struct luaL_Reg funcs[] =
    {
        {"parse", JSON_parse},
        {"stringify", JSON_stringify},
        {NULL, NULL}
    };
    luaL_register(L, "json", funcs);

    return 1;
}

