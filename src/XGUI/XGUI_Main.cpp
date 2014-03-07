
#include "XGUI_Workshop.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Install translator
    QTranslator aTranslator;
    aTranslator.load(app.applicationDirPath() + "\\XGUI_msg_en.qm");
    app.installTranslator(&aTranslator);

    XGUI_Workshop aWorkshop;
    aWorkshop.startApplication();

    return app.exec();
}
