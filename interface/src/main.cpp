//
//  main.cpp
//  interface/src
//
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Application.h"

#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <SharedUtil.h>

// DEBUG!!!!!!
#include "DatagramProcessor.h"

int main(int argc, const char * argv[]) {
    QElapsedTimer startupTime;
    startupTime.start();
    
    // Debug option to demonstrate that the client's local time does not 
    // need to be in sync with any other network node. This forces clock 
    // skew for the individual client
    const char* CLOCK_SKEW = "--clockSkew";
    const char* clockSkewOption = getCmdOption(argc, argv, CLOCK_SKEW);
    if (clockSkewOption) {
        int clockSkew = atoi(clockSkewOption);
        usecTimestampNowForceClockSkew(clockSkew);
        qDebug("clockSkewOption=%s clockSkew=%d", clockSkewOption, clockSkew);
    }
    
    int exitCode;
    {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        Application app(argc, const_cast<char**>(argv), startupTime);

        QTranslator translator;
        translator.load("interface_en");
        app.installTranslator(&translator);
    
        qDebug( "Created QT Application.");
        exitCode = app.exec();
    }
    qDebug("Normal exit.");

int s = 0;
for (int i = 0; i < DatagramProcessor::I; i++) {

    switch (DatagramProcessor::diffsI[i]) {
    case -2:
        printf("\nskew: %d\n", DatagramProcessor::skewsI[s++]);
        printf("prev:::::::::::::::::::::::::::::::\n");
        break;
    case -1:
        printf("curr:::::::::::::::::::::::::::::::\n");
        break;
    default:
        printf("\t type: %d  diff: %d\n", DatagramProcessor::typesI[i], DatagramProcessor::diffsI[i]);
        break;
    }
}



    return exitCode;
}   
