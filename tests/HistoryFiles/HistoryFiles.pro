DEV_PATH = $${PWD}/../../..
INCLUDEPATH *= $${DEV_PATH}/andyk
DEPENDPATH  *= $${DEV_PATH}/andyk
LIBS        *= -L$${DEV_PATH}/andyk/lib -landyk-core

INCLUDEPATH *= $${DEV_PATH}/history
DEPENDPATH  *= $${DEV_PATH}/history

CONFIG += \
    warn_on \
    console \
    qtestlib

QT += xml

TARGET = Test_HistoryFiles
TEMPLATE = app

DESTDIR = $${DEV_PATH}/history/tests/tests_bin

HEADERS += \
    test_history_files.h \
    ../../history_files.h \
    ../../history_structs.h

SOURCES += \
    main.cpp \
    test_history_files.cpp \
    ../../history_files.cpp \
    ../../history_structs.cpp

RESOURCES += \
    resources.qrc
