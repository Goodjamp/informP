TEMPLATE = app
DEFINES += USE_TIMER
DEFINES += USE_QT

TARGET = maxTimer
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../src/lcdHWInterfaceTIMER.c \
    main.c \
    LCD.c

HEADERS += \
    ../inc/lcdHWInterfaceTIMER.h \
    LCD.h

