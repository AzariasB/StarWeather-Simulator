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
 * File:   Simulator.cpp
 * Author: azarias
 *
 * Created on 31/12/2018
 */
#include "Simulator.hpp"

QByteArray Simulator::SUCCESS = QByteArray(1, SUCCESS_BIT);

Simulator::Simulator(QSerialPort &port, QObject *parent) : QObject(parent),
    m_sensor1(500),
    m_sensor2(600),
    m_sensor3(700),
    m_port(port)
{
    m_mode2Timer.setSingleShot(false);
    m_mode2Timer.setInterval(30);

    connect(&port, &QSerialPort::readyRead, [&](){
        QByteArray data = port.readAll();
        this->readCommand(data);
    });

    connect(&m_mode2Timer, &QTimer::timeout, [&](){ this->sendAllValues(); });

    connect(&m_sensor1, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 1); });
    connect(&m_sensor2, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 2); });
    connect(&m_sensor3, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 3); });
}

inline QByteArray Simulator::toData(qint16 value, qint8 sensorId) const
{
    qint16 data = static_cast<qint16>((static_cast<qint16>(sensorId & 0b11) << 10) | (value & TEN_BITS));
    qint8 firstBits = static_cast<qint8>(data & 0xFF);
    qint8 lastBits = static_cast<qint8>( (data & 0xFF00) >> 8 );
    QByteArray res;
    res.append(static_cast<char>(0));//Indicate we send a value
    res.append(lastBits);
    res.append(firstBits);
    return res;
}

void Simulator::receiveValue(qint16 value, qint8 sensorId)
{
    QByteArray realData = toData(value, sensorId);
    switch (m_mode) {
    case WORKING_MODE::NO_MODE:return;
    case WORKING_MODE::MODE_1:
        m_port.write(realData);
        m_port.flush();
        return;
    case WORKING_MODE::MODE_2:
    case WORKING_MODE::MODE_3:
        if(m_values.size() >= MAX_VALUES){
            m_values.removeFirst();
        }
        m_values.append(realData);
        return;
    }
}

void Simulator::sendAllValues()
{
    QByteArray values;
    values.setNum(m_values.size());
    m_port.write(values);
    for(const auto &val : m_values){
        m_port.write(val);
    }
    m_port.flush();
    m_values.clear();
}

QByteArray Simulator::setCurrentMode(WORKING_MODE nwMode)
{
    if(nwMode == m_mode){
        return QByteArray(1, MODE_ALREADY_STARTED);
    }
    if(nwMode != WORKING_MODE::NO_MODE && m_mode != WORKING_MODE::NO_MODE){
        return QByteArray(1, OTHER_MODE_RUNNING);
    }
    m_mode = nwMode;
    return SUCCESS;
}

void Simulator::readCommand(const QByteArray &array)
{
    if(array.size() < 2) return;
    qint8 first = array.at(0);
    qint8 data = array.at(1);
    qWarning() << "Received command " << QString::number(first) << " - with data : " << QString::number(data);
    switch (first) {
    case 0x0:
        m_port.write(setCurrentMode(WORKING_MODE::NO_MODE));
        break;
    case 0x1:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_1));
        break;
    case 0x2:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_2));
        return;
    case 0x3:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_3));
        break;
    case 0x4:
        sendAllValues();
        return;
    case 0x5:
        m_sensor1.setEmitingSpeed(data);
        m_port.write(SUCCESS);
        break;
    case 0x6:
        m_sensor2.setEmitingSpeed(data);
        m_port.write(SUCCESS);
        break;
    case 0x7:
        m_sensor3.setEmitingSpeed(data);
        m_port.write(SUCCESS);
        break;
    case 0x8:
        m_mode2Timer.setInterval(data);
        m_port.write(SUCCESS);
        break;
    }
    m_port.flush();
}
