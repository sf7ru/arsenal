// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <stdio.h>
#include <Json.h>
#include <axstring.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


typedef enum
{
    PHASE_none          =  0,
    PHASE_name,
    PHASE_equals,
    PHASE_value,
} PHASE;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      JsonParser::JsonParser
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
JsonParser::JsonParser()
{

}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      JsonParser::~JsonParser
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
JsonParser::~JsonParser()
{

}
// ***************************************************************************
// FUNCTION
//      JsonParser::addChar
// PURPOSE
//
// PARAMETERS
//      CHAR c --
// RESULT
//      void --
// ***************************************************************************
void JsonParser::addChar(CHAR           c)
{
    switch (phase)
    {
        case PHASE_name:
            axstr_add_char(name, c);
            break;

        case PHASE_value:
            axstr_add_char(value, c);
            break;

        default:
            break;
    }
}
PCSTR JsonParser::notNullName()
{
    PCSTR           result          = axstr_c(name);

    ENTER(true);

    if (!strlen(result))
    {
        result = getUnnamedName();
    }

    RETURN(result);
};
// ***************************************************************************
// FUNCTION
//      JsonParser::parseObject
// PURPOSE
//
// PARAMETERS
//      AXDomObject * o      --
//      PCSTR         source --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR JsonParser::parseObject(AXDomObject *  o,
                              PCSTR source)
{
#define PUSH_PROPERTY(a)    if ((p = o->addProperty(axstr_c(name))) != nil) { axstr_truncate(name, 0); phase = PHASE_##a; }
#define PUSH_VALUE(a)       if (p) { p->setValue((PCSTR)axstr_c(value)); p = nil; } phase = PHASE_##a; axstr_truncate(value, 0);
#define PARSE_OBJECT(a)     if ((no = o->addObject(axstr_c(name))) != nil) { no->setType(AXDOMT_##a); on = parseObject(no, ++on); }



    PCSTR                   on              = source;
    BOOL                    esc             = false;
    AXDomProperty *         p               = nil;
    AXDomObject *           no              = nil;
    BOOL                    run             = true;
    BOOL                    braces          = false;

    ENTER(true);

    phase   = PHASE_name;

    axstr_truncate(name, 0);
    axstr_truncate(value, 0);

    while (run && *on)
    {
        //printf("CH1 %c\n", *on);

        if (esc)
        {
            addChar(*on);

            esc = false;
        }
        else if (*on != '\"' && braces)
        {
            switch (*on)
            {
                case '\\':
                    esc = true;
                    break;

                default:
                    addChar(*on);
                    break;
            }
        }
        else
        {
            // printf("CH2 %c\n", *on);
            switch (*on)
            {
                case '\r':
                case '\n':
                case ' ':
                case '\t':
                    break;

                case '\\':
                    esc = true;
                    break;

                case '\"':
                    switch (phase)
                    {
                        case PHASE_equals:
                            PUSH_PROPERTY(value);
                            break;
                    }
                    braces = !braces;
                    break;

                case ':':
                    phase = PHASE_equals;
                    break;

                case ',':
                    switch (phase)
                    {
                        case PHASE_value:
                            PUSH_VALUE(name);
                            break;

                        default:
                            break;
                    }
                    break;

                case '[':
                    switch (phase)
                    {
                        case PHASE_equals:
                            PARSE_OBJECT(array);
                            break;

                        default:
                            // ERROR
                            break;
                    }
                    break;

                case '{':
                    //printf("JsonParser.cpp(204): phase = %d\n", phase);
                    switch (phase)
                    {
                        case PHASE_name:
                        case PHASE_equals:
                            //printf("JsonParser.cpp(209): stage 1\n");
                            PARSE_OBJECT(object);
                            break;

                        default:
                            //printf("JsonParser.cpp(209): stage 2\n");
                            // ERROR
                            break;
                    }
                    break;

                case ']':
                case '}':
                    PUSH_VALUE(name);
                    run = false;
                    break;


                default:
                    switch (phase)
                    {
                        case PHASE_equals:
                            PUSH_PROPERTY(value);
                            addChar(*on);
                            break;

                        default:
                            addChar(*on);
                            break;
                    }
                    break;
            }
        }


        if (*on && run)
            on++;
    }

    //printf("<< parseObject %c %d\n", *on, run);

    RETURN(on);
}
// ***************************************************************************
// FUNCTION
//      JsonParser::parse
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      JsonSheet * --
// ***************************************************************************
JsonSheet * JsonParser::parse(PCSTR          source)
{
    JsonSheet *             result          = nil;
    PCSTR                   on              = source;

    ENTER(true);

    if ((result = JsonSheet::create()) != nil)
    {
        if ((name = axstr_create(100)) != nil)
        {
            if ((value = axstr_create(100)) != nil)
            {
                parseObject(result, on);

                axstr_destroy(value);
            }

            axstr_destroy(name);
        }
    }

    RETURN(result);
}
