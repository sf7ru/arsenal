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

#include <stdarg.h>

#include <AXGtkApp.h>

#include <app.h>

#ifdef CLASS_AXGTKAPP
#include CLASS_INCLUDE(AXGTKAPP)
#endif

#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

#include <IDs.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DETACHED_SELF       1
#define DETACHED_P1         2
#define DETACHED_P2         4

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXGtkApp::AXGtkApp
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXGtkApp::AXGtkApp()
{
    mainWindow       = nil;
    gtkBuilder       = nil;
#ifdef RES_xml_main_window 
    mainWindowResId  = RES_xml_main_window;
#else
    mainWindowResId  = (UINT)-1;
#endif 
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXGtkApp::~AXGtkApp
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXGtkApp::~AXGtkApp()
{
    if (gtkBuilder)
        g_object_unref (G_OBJECT (gtkBuilder));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      axgtkapp_main_quit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
GTK_SIGNAL_HANDLER void axgtkapp_main_quit()
{
#ifdef CLASS_AXGTKAPP
    CLASS_AXGTKAPP & app = CLASS_AXGTKAPP::getInstance();

    if (app.onGuiQuitSignal())
#endif
    {
        gtk_main_quit();
    }
}
BOOL AXGtkApp::guiInit()
{
    BOOL            result          = false;
    GError*         error           = nil;
    PCSTR           xml;
    UINT            size;

    ENTER(true);

    if ((gtkBuilder = gtk_builder_new()) != nil)
    {
        if (mainWindowResId != (UINT)-1)
        {
            if ((xml = resPSTR(mainWindowResId, &size)) != nil)
            {
                if (gtk_builder_add_from_string(gtkBuilder, xml, size, &error))
                {
                    gtk_builder_connect_signals (gtkBuilder, NULL);

                    if ((mainWindow = GTK_WIDGET (gtk_builder_get_object (gtkBuilder, "mainwindow"))) != nil)
                    {
                        g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(axgtkapp_main_quit), NULL);

                        result = true;
                    }
                }
                else
                {
                    g_critical ("cannot parse UI: %s", error->message);
                    g_error_free (error);
                }

                resFree(mainWindowResId);
            }
            else
            {
                printf("ERROR: cannot load UI, resource does not exist\n");
            }
        }
        else
        {
            printf("ERROR: cannot load UI, the resource identifier of the main window is not specified.\n");
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::appInit
// PURPOSE
//
// PARAMETERS
//      int    argc   --
//      char * argv[] --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXGtkApp::appInit(int            argc,
                       char *         argv[])
{
    BOOL            result          = false;

    ENTER(true);

    mainThread = gettid();

    if (onGuiInitStart())
    {
        gtk_init(&argc, &argv);

        if (initResources(nil))
        {
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Keymaster::appMain
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      int --
// ***************************************************************************
int AXGtkApp::appMain(void)
{
    int         result      = 1;

    gtk_widget_show(mainWindow);

    if (onGuiInitOver())
    {
        gtk_main();

        result = 0;
    }
//    else
//        gtk_main_quit();

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::findWidget
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      GObject* --
// ***************************************************************************
GObject* AXGtkApp::findWidget(PCSTR          name)
{
    return gtk_builder_get_object(GTK_BUILDER(gtkBuilder), name);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::messageYesNo
// PURPOSE
//
// PARAMETERS
//      PCSTR msg --
// RESULT
//      GtkResponseType --
// ***************************************************************************
GtkResponseType AXGtkApp::messageYesNo(PCSTR   msg)
{
    GtkResponseType result          = GTK_RESPONSE_NONE;
    GtkWidget *     dialog;

    ENTER(true);

    if ((dialog = gtk_message_dialog_new(
                    GTK_WINDOW(mainWindow),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_QUESTION,
                    GTK_BUTTONS_YES_NO,
                    "%s", msg)) != nil)
    {
        result = (GtkResponseType)gtk_dialog_run(GTK_DIALOG(dialog));

        gtk_widget_destroy(dialog);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::messageOk
// PURPOSE
//
// PARAMETERS
//      PCSTR msg --
// RESULT
//      void --
// ***************************************************************************
void AXGtkApp::messageOk(PCSTR   msg)
{
    GtkWidget *     dialog;

    ENTER(true);

    if ((dialog = gtk_message_dialog_new(
                    GTK_WINDOW(mainWindow),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "%s", msg)) != nil)
    {
        (GtkResponseType)gtk_dialog_run(GTK_DIALOG(dialog));

        gtk_widget_destroy(dialog);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::post
// PURPOSE
//
// PARAMETERS
//      GSourceFunc func --
//      PAXGTKAPPCD data --
// RESULT
//      INT --
// ***************************************************************************
INT AXGtkApp::postOnce(GSourceFunc    func,
                       PAXGTKAPPCD    data)
{
    INT             result          = 0;
    PAXGTKAPPCD     pass;

    ENTER(true);

    if (gettid() != mainThread)
    {
        if ((pass = (PAXGTKAPPCD)strz_memdup(data, sizeof(AXGTKAPPCD))) != nil)
        {
            pass->detached  = DETACHED_SELF;
            result          = gdk_threads_add_idle(func, pass);
        }
    }
    else
    {
        data->detached = false;
        (*func)(data);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::loadFromResource
// PURPOSE
//
// PARAMETERS
//      UINT                     resId --
//      UINT           width   = 0     --
//      UINT           height  = 0     --
// RESULT
//      GdkPixbuf * --
// ***************************************************************************
GdkPixbuf * AXGtkApp::loadFromResource(UINT           resId,
                                       UINT           width,
                                       UINT           height)
{
    GdkPixbuf *         result          = nil;
    GdkPixbufLoader *   loader;
    PCVOID              data;
    UINT                size;

    ENTER(true);

    if ((data = resBIN(resId, &size)) != nil)
    {
        if ((loader = gdk_pixbuf_loader_new()) != nil)
        {
            gdk_pixbuf_loader_write (loader, (guint8*)data, size, nil);

            if ((result = gdk_pixbuf_loader_get_pixbuf(loader)) != nil)
            {
                if (width && height)
                {
                    result = gdk_pixbuf_scale_simple(result, width, height, GDK_INTERP_BILINEAR);
                }

            }

            gdk_pixbuf_loader_close(loader, nil);
        }

        resFree(resId);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::message
// PURPOSE
//
// PARAMETERS
//      UINT  type    --
//      UINT  buttons --
//      PCSTR title   --
//      PCSTR message --
//            ...     --
// RESULT
//      GtkResponseType --
// ***************************************************************************
GtkResponseType AXGtkApp::message(GtkWidget **   ndialog,
                                  GtkMessageType type,
                                  GtkButtonsType buttons,
                                  PCSTR          title,
                                  PCSTR          message,
                                  ...)
{
    GtkResponseType result          = GTK_RESPONSE_NONE;
    PSTR            msg;
    va_list         list;
    GtkWidget *     dialog;

    ENTER(true);

    if ((msg = (PSTR)MALLOC(AXLHUGE)) != nil)
    {
        va_start(list, message);

        vsnprintf(msg, AXLHUGE, message, list);

        va_end(list);

        if ((dialog = gtk_message_dialog_new(
                        GTK_WINDOW(mainWindow),
                        GTK_DIALOG_MODAL,
                        type,
                        buttons,
                        "%s", msg)) != nil)
        {
            *ndialog    = dialog;
            result      = (GtkResponseType)gtk_dialog_run(GTK_DIALOG(dialog));
            *ndialog    = nil;

            gtk_widget_destroy(dialog);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::dialogLoad
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      GtkWidget * --
// ***************************************************************************
GtkWidget * AXGtkApp::dialogLoad(PCSTR          name)
{
    return GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(gtkBuilder), name));
}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::dialogDestroy
// PURPOSE
//
// PARAMETERS
//      GtkWidget * dlg --
// RESULT
//      GtkWidget * --
// ***************************************************************************
GtkWidget * AXGtkApp::dialogDestroy(GtkWidget *    dlg)
{
    gtk_widget_destroy(dlg);

    return nil;
}
GtkResponseType AXGtkApp::dialogRun(GtkWidget *    dlg)
{
    GtkResponseType     result;

    ENTER(true);

    result = (GtkResponseType)gtk_dialog_run(GTK_DIALOG(dlg));

    RETURN(result);
}
void AXGtkApp::dialogHide(GtkWidget *    dlg)
{
    gtk_widget_hide(dlg);
}
//void AXGtkApp::dialog(PCSTR          name)
//{
//    GtkWidget *     dlg;
////    GError*         error           = nil;
////    PCSTR           xml;
////    UINT            size;
//
////    if ((gtkBuilder = gtk_builder_new()) != nil)
////    {
//    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ AXGtkApp::dialog stage 1\n");
//    if ((dlg = GTK_WIDGET (gtk_builder_get_object(GTK_BUILDER(gtkBuilder), name))) != nil)
//    {
//        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ AXGtkApp::dialog stage 2\n");
//        (GtkResponseType)gtk_dialog_run(GTK_DIALOG(dlg));
//        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ AXGtkApp::dialog stage 3\n");
//
//        //g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);
//
//        gtk_widget_hide(dlg);
//
//        //result = true;
//    }
//    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ AXGtkApp::dialog stage 4\n");
////            }
////            else
////            {
////                g_critical ("cannot parse UI: %s", error->message);
////                g_error_free (error);
////            }
////
////            resFree(resId);
////        }
////        else
////        {
////            printf("ERROR: cannot load UI resource\n");
////        }
//////    }
//
//}
// ***************************************************************************
// FUNCTION
//      AXGtkApp::dataUnref
// PURPOSE
//
// PARAMETERS
//      PAXGTKAPPCD data --
// RESULT
//      void --
// ***************************************************************************
void AXGtkApp::dataUnref(PAXGTKAPPCD    data)
{
    if (data->detached & DETACHED_P1)
    {
        SAFEFREE(data->pparam1);
    }

    if (data->detached & DETACHED_P2)
    {
        SAFEFREE(data->pparam2);
    }

    if (data->detached & DETACHED_SELF)
    {
        FREE(data);
    }
}
