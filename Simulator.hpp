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

constexpr int MAX_VALUES = std::numeric_limits<int>::max();
constexpr qint8 SUCCESS_BIT = 0x1;
constexpr qint8 MODE_ALREADY_STARTED = 0x2;
constexpr qint8 OTHER_MODE_RUNNING = 0x3;

enum class WORKING_MODE : qint8 {
    NO_MODE = 0x0,
    MODE_1 = 0x1,
    MODE_2 = 0x2,
    MODE_3 = 0x3,
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

    QVector<QByteArray> m_values;

    WORKING_MODE m_mode = WORKING_MODE::NO_MODE;

    void readCommand(const QByteArray &array);

    void receiveValue(qint16 value, qint8 sensorId);

    inline QByteArray toData(qint16 value, qint8 sensorId) const;

    void sendAllValues();

    QByteArray setCurrentMode(WORKING_MODE nwMode);

    static QByteArray SUCCESS;
};

