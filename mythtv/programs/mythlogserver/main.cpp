// -*- Mode: c++ -*-

// C++ headers
#include <unistd.h>
#include <iostream>
using namespace std;

// Qt headers
#include <QCoreApplication>
#include <QApplication>
#include <QString>
#include <QtCore>
#include <QtGui>

// MythTV headers
#include "mythccextractorplayer.h"
#include "commandlineparser.h"
#include "mythcontext.h"
#include "mythversion.h"
#include "programinfo.h"
#include "ringbuffer.h"
#include "exitcodes.h"
#include "loggingserver.h"

namespace {
    void cleanup()
    {
        delete gContext;
        gContext = NULL;
    }

    class CleanupGuard
    {
      public:
        typedef void (*CleanupFunc)();

      public:
        CleanupGuard(CleanupFunc cleanFunction) :
            m_cleanFunction(cleanFunction) {}

        ~CleanupGuard()
        {
            m_cleanFunction();
        }

      private:
        CleanupFunc m_cleanFunction;
    };
}

static void qt_exit(int)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    QCoreApplication::exit(GENERIC_EXIT_OK);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName(MYTH_APPNAME_MYTHLOGSERVER);

    MythLogServerCommandLineParser cmdline;
    if (!cmdline.Parse(argc, argv))
    {
        cmdline.PrintHelp();
        return GENERIC_EXIT_INVALID_CMDLINE;
    }

    if (cmdline.toBool("showhelp"))
    {
        cmdline.PrintHelp();
        return GENERIC_EXIT_OK;
    }

    if (cmdline.toBool("showversion"))
    {
        cmdline.PrintVersion();
        return GENERIC_EXIT_OK;
    }

    //QString pidfile = cmdline.toString("pidfile");
    int retval = cmdline.Daemonize();
    if (retval != GENERIC_EXIT_OK)
        return retval;

    bool daemonize = cmdline.toBool("daemon");
    QString mask("general");
    if ((retval = cmdline.ConfigureLogging(mask, daemonize)) != GENERIC_EXIT_OK)
        return retval;

    if (daemonize)
        // Don't listen to console input if daemonized
        close(0);

    CleanupGuard callCleanup(cleanup);
    signal(SIGINT, qt_exit);
    signal(SIGTERM, qt_exit);

    gContext = new MythContext(MYTH_BINARY_VERSION);
    if (!gContext->Init(false))
    {
        cerr << "Failed to init MythContext, exiting." << endl;
        return GENERIC_EXIT_NO_MYTHCONTEXT;
    }

    logServerStart();

    while (true)
        usleep(100000);

    return GENERIC_EXIT_OK;
}


/* vim: set expandtab tabstop=4 shiftwidth=4: */
