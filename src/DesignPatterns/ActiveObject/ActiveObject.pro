TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cc

HEADERS += \
    MethodRequest.h \
    ActivationQueue.h \
    MQScheduler.h \
    Future.h \
    MQServant.h \
    MQProxy.h
