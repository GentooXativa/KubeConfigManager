#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("GentooXativa");
    QCoreApplication::setApplicationName("KubeConfManager");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "KubeConfManager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow mainWin;

    // if (!parser.positionalArguments().isEmpty())
    //     mainWin.loadFile(parser.positionalArguments().first());

    mainWin.show();
    return app.exec();
}