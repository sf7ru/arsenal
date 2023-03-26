
#include <arsenal.h>

static int _char2code(CHAR c)
{
    int     code    = -1;

    if (c >= '0')
    {
        if (c >= 'A')
        {
            if (c >= 'a')
            {
                if (c <= 'z')
                {
                    code = c - 71;
                }
            }
            else if (c <= 'Z')
            {
                code = c - 'A';
            }
        }
        else if (c <= '9')
        {
            code = c + 4;
        }
        else if (c == '=')
        {
            code = -3;
        }
    }
    else switch(c)
    {
        case '+' : code = c + 19; break;
        case '/' : code = c + 16; break;
        case ' ' :
        case '\t':
        case '\r':
        case '\n': code = -2; break;
    }

//    printf("char '%c' = code %d\n", c, code);

    return code;
}
UINT base64_dec(PVOID    tgt,
                UINT     tgtsize,
                PCSTR    src)
{
    UINT        size    = 0;
    PCSTR       on;
    PU8       to;
    UINT        left;
    U32      buff    = 0;
    int         tail    = 0;
    int         count;
    int         code;

    on  = src;
    to  = tgt;
    left = tgtsize;
    count = 0;

    while (left && *on)
    {
        if (count < 4)
        {
            switch (code = _char2code(*(on++)))
            {
                case -1: // error
                    left = 0;
                    size = 0;
                    break;

                case -2: // white space
                    break;

                case -3: // tail symbol
                    count++;
                    tail++;
                    buff <<= 6;
                    break;

                default:
                    if (!tail)
                    {
                        count++;
                        buff <<= 6;
                        buff  |= code;
                    }
                    break;
            }
        }

        if (count == 4)
        {
            for (count--; left && (count > tail); count--)
            {
                *(to++) = (U8)((buff >> ((count - 1) * 8)) & 0xff);
                left--;
                size++;
            }

            count -= tail;
        }
    }

    return size;
}
