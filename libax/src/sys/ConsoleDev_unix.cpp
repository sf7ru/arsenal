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
#include <unistd.h>

#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>

#include <axtime.h>

#include <pvt_unix.h>

#include <ConsoleDev.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      ConsoleDev::ConsoleDev
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
ConsoleDev::ConsoleDev()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

//    setbuf(stdin, NULL);
//    setbuf(stdout, NULL);
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      ConsoleDev::~ConsoleDev
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
ConsoleDev::~ConsoleDev()
{
}
// ***************************************************************************
// FUNCTION
//      ConsoleDev::read
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
//      UINT  TO   --
// RESULT
//      INT --
// ***************************************************************************
INT ConsoleDev::read(PVOID          data,
                     INT            size,
                     UINT           TO)
{
    INT                 result          = 0;

    ENTER(true);

    result = _ax_nonblock_dev_read(STDIN_FILENO, data, size, TO);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      ConsoleDev::write
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT ConsoleDev::write(PCVOID         data,
                      INT            size,
                      UINT           TO)
{
    return ::write(STDOUT_FILENO, data, size);
}
