#include "test_history_files.h"


int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::RussianFederation));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);
    Test_HistoryFiles test;
    return QTest::qExec(&test);
}

