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

#ifndef __AXGTKAPP_H__
#define __AXGTKAPP_H__

#include <AXApp.h>

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <IDs.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define GTK_SIGNAL_HANDLER extern "C" __attribute__((used, visibility("default")))

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// Call data for static
typedef struct _tag_AXGTKAPPCD
{
        PVOID               pcl;
        UINT                uparam1;
        UINT                uparam2;
        PVOID               pparam1;
        PVOID               pparam2;
        UINT                detached;
} AXGTKAPPCD, * PAXGTKAPPCD;



// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXGtkApp: public AXApp
{
protected:
        GtkWidget *     mainWindow;
        GtkBuilder *    gtkBuilder;
        pid_t           mainThread;
        UINT            mainWindowResId;

        BOOL            guiInit                 ();

        BOOL            appInit                 (int            argc,
                                                 char *         argv[]);

        GObject*        findWidget              (PCSTR          name);

        INT             postOnce                (GSourceFunc    func,
                                                 PAXGTKAPPCD    data);


        GdkPixbuf *     loadFromResource        (UINT           resId,
                                                 UINT           width   = 0,
                                                 UINT           height  = 0);

public:
                        AXGtkApp                ();

                        ~AXGtkApp               ();

        int             appMain                 (void);

        GtkResponseType messageYesNo           (PCSTR          message);

        void            messageOk              (PCSTR          message);

        GtkResponseType message                 (GtkWidget **   dialog,
                                                 GtkMessageType type,
                                                 GtkButtonsType buttons,
                                                 PCSTR          title,
                                                 PCSTR          message,
                                                 ...);

virtual BOOL            onGuiInitStart          ()
        { return true; }

virtual BOOL            onGuiInitOver           ()
        { return true; }

virtual BOOL            onGuiQuitSignal         ()
        { return true; }

        GtkWidget *     dialogLoad              (PCSTR          name);

        GtkResponseType dialogRun               (GtkWidget *    dlg);

        void            dialogHide              (GtkWidget *    dlg);

        GtkWidget *     dialogDestroy           (GtkWidget *    dlg);


static  void            dataUnref               (PAXGTKAPPCD    data);

//        BOOL            isUiThread              ();

static  GtkWidget *     widget_destroy          (GtkWidget *    w)
        { gtk_widget_destroy(w); return nil; }

};

#endif // __AXGTKAPP_H__
