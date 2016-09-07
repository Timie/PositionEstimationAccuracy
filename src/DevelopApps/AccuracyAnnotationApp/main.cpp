#include <QApplication>
#include <QString>

#include "rce/gui/RDistanceAnnotationWidget.h"
#include "dfs/core/DDebug.h"

int main(int argc,
         char *argv[])
{

    try
    {
        QApplication app(argc,
                         argv);

        app.setApplicationName("AccuracyAnnotation");
        app.setOrganizationDomain("vutbr.cz");
        app.setOrganizationName("VUT");




        rce::gui::RDistanceAnnotationWidget *w = new rce::gui::RDistanceAnnotationWidget(NULL);
        w->show();




        return app.exec();
    }
    catch(const std::exception &e)
    {
        dWarning() << "Exception:" << e.what();
        return -1;
    }
    catch(const QString &e)
    {
        dWarning() << "Exception:" << e;
        return -2;
    }
    catch(const char *e)
    {
        dWarning() << "Exception:" << e;
        return -3;
    }
}
