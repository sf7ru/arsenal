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
#include <arsenal.h>
#include <A7FileWatch.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define ROOT_PATH           "/home/ak/v/ra1n/cinelab/outbox"


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class   MyWatch:        public A7FileWatch
{
public:
        void            onEventFileCreate       (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFileDelete       (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFileModify       (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFileMovedTo      (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFileMovedFrom    (PCSTR          path,
                                                 PCSTR          name);



        void            onEventFolderCreate     (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFolderDelete     (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFolderModify     (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFolderMovedTo    (PCSTR          path,
                                                 PCSTR          name);

        void            onEventFolderMovedFrom  (PCSTR          path,
                                                 PCSTR          name);


};

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

MyWatch                     fw;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

void MyWatch::onEventFileCreate(PCSTR          path,
                                PCSTR          name)
{
    printf("FILE CREATE: '%s/%s'\n", path, name);
}
void MyWatch::onEventFileDelete(PCSTR          path,
                                 PCSTR          name)
{
    printf("FILE DELETE: '%s/%s'\n", path, name);
}
void MyWatch::onEventFileModify(PCSTR          path,
                                PCSTR          name)
{
    printf("FILE MODIFY: '%s/%s'\n", path, name);
}
void MyWatch::onEventFileMovedFrom(PCSTR          path,
                                   PCSTR          name)
{
    printf("FILE MOVED FROM: '%s/%s'\n", path, name);
}
void MyWatch::onEventFileMovedTo(PCSTR          path,
                                 PCSTR          name)
{
    printf("FILE MOVED TO: '%s/%s'\n", path, name);
}

void MyWatch::onEventFolderCreate(PCSTR          path,
                                PCSTR          name)
{
    printf("FOLDER CREATE: '%s/%s'\n", path, name);
}
void MyWatch::onEventFolderDelete(PCSTR          path,
                                 PCSTR          name)
{
    printf("FOLDER DELETE: '%s/%s'\n", path, name);
}
void MyWatch::onEventFolderModify(PCSTR          path,
                                PCSTR          name)
{
    printf("FOLDER MODIFY: '%s/%s'\n", path, name);
}
void MyWatch::onEventFolderMovedFrom(PCSTR          path,
                                   PCSTR          name)
{
    printf("FOLDER MOVED FROM: '%s/%s'\n", path, name);
}
void MyWatch::onEventFolderMovedTo(PCSTR          path,
                                 PCSTR          name)
{
    printf("FOLDER MOVED TO: '%s/%s'\n", path, name);
}
// ***************************************************************************
// FUNCTION
//      main
// PURPOSE
//
// PARAMETERS
//      int      argc   --
//      char *   argv[] --
// RESULT
//      int  --
// ***************************************************************************
int main(int        argc,
         char *     argv[])
{
    int             result      = 0;

    ENTER(TRUE);

    printf("monitorign for path '%s'...\n", ROOT_PATH);

    if (fw.init(ROOT_PATH))
    {
        while (true)
        {
            fw.check(100);
        }
    }

    RETURN(result);
}
