#include "history_app.h"
#include "history_dlg.h"

// -----------------------------------------------------------------------------------------------------------
// DeformApp
// -----------------------------------------------------------------------------------------------------------
QDialog* HistoryApp::findDialog(QWidget* pWidget)
{
    QDialog* pDialog = qobject_cast<QDialog*>(pWidget);
    while(pDialog == 0 && pWidget!=0)
    {
        pWidget = pWidget->parentWidget();
        pDialog = qobject_cast<QDialog*>(pWidget);
    }
    return pDialog;
}

// -----------------------------------------------------------------------------------------------------------
HistoryApp::HistoryApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
}

HistoryApp::~HistoryApp()
{
}

// -----------------------------------------------------------------------------------------------------------
bool HistoryApp::notify(QObject* receiver, QEvent* event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (QSharedPointer<ErrorBase> error)
    {
        QString msg = error->toString() + "\nDo you want to terminate the application?";
        QWidget* pWidget = qobject_cast<QWidget*>(receiver);
        QDialog* pDialog = findDialog(pWidget);

        int ret = QMessageBox::critical(pDialog, "Error", msg,
                                        QMessageBox::Yes|QMessageBox::Default, QMessageBox::No);
        if (ret == QMessageBox::Yes)
            exit(-1);

        return false;
    }
}

// -----------------------------------------------------------------------------------------------------------
void HistoryApp::xmlParse(const XmlManager& manager)
{
    XmlParser::xmlParse(manager);
    testNames("History_v1.0", "History");
    addCollection<Ticker>("Ticker");
    addCollection<TickersGroup>("TickersGroup");
    addCollection<FilesGroup>("FilesGroup");
    addCollection<StockFileManager>("StockFileManager");
}

// -----------------------------------------------------------------------------------------------------------
int HistoryApp::exec()
{
    //try
    {
        Loger::instance("history.txt");
        Loger::log("History was started", "", Loger::sLevel0);

//        QStringList args = arguments();
//        if (args.size() == 2 && args.at(1) == "-c")
//            validityTestDB();
//        if (args.size() == 2 && args.at(1) == "-t")
//            testTariffManager();

        // config load
        XmlManager mng;
        mng.parseFile("history.xml", this);

        // watch
        m_stockFileManagers["TextFileManager"]->watch();
        m_stockFileManagers["DataFileManager"]->watch();
        m_stockFileManagers["CacheFileManager"]->watch();

        // создание gui
        HistoryDlg dlg;
        dlg.resize(800, 600);

        // обработка цикла сообщений
        return dlg.exec();
        return 0;
    }
//    catch (QSharedPointer<ErrorBase> error)
//    {
//        QString msg = error->toString() + "\nThe application will be terminated.";
//        QMessageBox::critical(0, "Error", msg, QMessageBox::Ok);
//        return -1;
//    }
}

// -----------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // locale
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::RussianFederation));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

#ifndef TESTING
    HistoryApp app(argc, argv);
    return app.exec();
#else
    QCoreApplication app(argc, argv);
    Test_Fixture t;
    QTest::qExec(&t, app.arguments());
    return 0;
#endif
}

// -----------------------------------------------------------------------------------------------------------
