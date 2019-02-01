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
 * File:   Simulator.hpp
 * Author: azarias
 *
 * Created on 31/12/2018
 */
#pragma once

#include <QObject>
#include <QVector>
#include <QtSerialPort>
#include "Sensor.hpp"

constexpr int MAX_VALUES = std::numeric_limits<quint16>::max();
constexpr quint16 SIZE_MASK = 0xFFFF;
constexpr qint8 SUCCESS_BIT = 0x0;
constexpr qint8 ERROR_BIT = 0x1;

constexpr qint16 SENSORID_MASK  = 0b0000000000000011;
constexpr qint16 FREQUENCY_MASK = 0b0000000000001111;
constexpr qint16 VALUE_MASK     = 0b0000001111111111;

enum class WORKING_MODE : qint8 {
    NO_MODE = 0x0,
    MODE_1 = 0x1,
    MODE_2 = 0x2,
    MODE_3 = 0x3,
};

enum WeatherCommand : quint8 {
    STOP_MODE = 0x0,
    START_MODE_1 = 0x1,
    START_MODE_2 = 0x2,
    START_MODE_3 = 0x3,
    GET_DATA = 0x4,
    CONFIGURE_FE_1 = 0x5,
    CONFIGURE_FE_2 = 0x6,
    CONFIGURE_FE_3 = 0x7,
    CONFIGURE_MODE_2 = 0x8,
    SEND_MODE1_DATA = 0x9,
    SEND_MODE2_DATA = 0xA
};


class Simulator : public QObject
{
    Q_OBJECT
public:
    explicit Simulator(QSerialPort &port, QObject *parent = nullptr);


private:
    Sensor m_sensor1;
    Sensor m_sensor2;
    Sensor m_sensor3;

    QSerialPort &m_port;

    QTimer m_mode2Timer;

    QByteArray m_values;

    bool m_started = false;

    WORKING_MODE m_mode = WORKING_MODE::NO_MODE;

    void readCommand(const QByteArray &array);

    void receiveValue(qint16 value, quint32 tmstp, qint8 sensorId);

    inline QByteArray toData(qint8 sendingMode, quint32 timestamp, qint16 value, qint8 sensorId) const;

    void sendAllValues(bool forced);

    QByteArray getFrequencies();

    QByteArray setCurrentMode(WORKING_MODE nwMode);

    bool sendByte(quint8 byte);

    bool sendBytes(QVector<quint8> bytes);

    bool sendBytes(const QByteArray &bytes);

    static QByteArray success(qint8 command);

    static QByteArray failure(qint8 command);

};

