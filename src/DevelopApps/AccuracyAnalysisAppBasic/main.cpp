#include <iostream>

#include <QSettings>
#include <QCoreApplication>

#include "rce/accuracy/RVisionExperiment.h"
#include "rce/accuracy/RCSVOutput.h"
#include "dfs/core/DDebug.h"

int
main(int argc,
     char *argv[])
{
    try
    {
        QCoreApplication app(argc, argv);
        QSettings settings("experiment_config_basic.ini",
                           QSettings::IniFormat);


        rce::accuracy::RVisionExperiment e;


        e.loadSettings(settings);

        e.run();

        return 0;
    }
    catch(const char *e)
    {
        dCritical() << "Exception:" << e;
        return -1;
    }
    catch(const std::exception &e)
    {
        dCritical() << "Exception:" << e.what();
        return -2;
    }
    catch(...)
    {
        dCritical() << "Unknown Exception";
        throw;
    }
}
