/*
 * The MIT License
 *
 * Copyright 2017-2018 azarias.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * File:   main.cpp
 * Author: azarias
 *
 * Created on 14/12/2018
 */
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QtSerialPort>

#include "Simulator.hpp"

/**
SETUP :
command 1 (WeatherStation folder) : socat PTY,link=./virtual-tty,raw,echo=0 -
command 2 (WeatherSimulator folder) : socat PTY,link=./arduino-sim,raw,echo=0 PTY,link=../../../WeatherStation/build/Debug/virtual-tty,raw,echo=0
command 3 (WeatherSimulator folder): ./WeatherSimulator
command 4 (WeatherStation folder): ./WeatherStation
 */


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSerialPort port("./arduino-sim");
    qWarning() << "Starting simulator ...";

    if(!port.open(QIODevice::ReadWrite)){
        qWarning() << port.errorString() << "\n" << port.error();
        return -1;
    }

    Simulator s(port, nullptr);

    /*
    QTimer timer;
    timer.setSingleShot(true);


    QObject::connect(&port, &QSerialPort::readyRead, [&port, &timer](){
        QByteArray data = port.readAll();
        qWarning() << QString(data);
        timer.start(0);
    });

    QObject::connect(&timer, &QTimer::timeout, [&port](){
        const char toSend[] = {'a','b'};
        port.write(toSend, 2);
        if(!port.flush()){
            qWarning() << "No data written\n";
        } else {
            qWarning() << "Data written";
        }
    }); */



    return a.exec();
}
