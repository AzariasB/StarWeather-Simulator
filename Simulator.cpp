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
#include <bitset>


Simulator::Simulator(QSerialPort &port, QObject *parent) : QObject(parent),
    m_sensor1(100),
    m_sensor2(200),
    m_sensor3(1000),
    m_port(port)
{
    m_mode2Timer.setSingleShot(false);
    m_mode2Timer.setInterval(3000);

    connect(&port, &QSerialPort::readyRead, [&](){
        QByteArray data = port.readAll();
        this->readCommand(data);
    });

    connect(&m_mode2Timer, &QTimer::timeout, [&](){ this->sendAllValues(false); });

    connect(&m_sensor1, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 1); });
    connect(&m_sensor2, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 2); });
    connect(&m_sensor3, &Sensor::sensedValue, [&](qint16 val){ this->receiveValue(val, 3); });
}

inline QByteArray Simulator::toData(qint8 sendingMode, qint16 value, qint8 sensorId) const
{
    std::bitset<16> data = sensorId & SENSORID_MASK;
    data <<= 10;
    data |= value & VALUE_MASK;

    QByteArray res;
    if(sendingMode == SEND_MODE1_DATA){
        res.append(char(sendingMode));//Indicate we send a value
    }
    ulong total = data.to_ulong();
    quint8 bigBits = quint8((total & 0xFF00) >> 8) & 0xFF;
    res.append(bigBits);
    quint8 smallBits = quint8( total & quint8(0x00FF));
    res.append(smallBits);
    return res;
}

void Simulator::receiveValue(qint16 value, qint8 sensorId)
{
    switch (m_mode) {
    case WORKING_MODE::NO_MODE:return;
    case WORKING_MODE::MODE_1:
        m_port.write(toData(SEND_MODE1_DATA, value, sensorId));
        m_port.flush();
        return;
    case WORKING_MODE::MODE_2:
        if(m_values.size() >= MAX_VALUES){
            m_values.remove(0,1);
        }
        m_values.append(toData(SEND_MODE2_DATA, value, sensorId));
        return;
    case WORKING_MODE::MODE_3:
        if(m_values.size() >= MAX_VALUES){
            m_values.remove(0, 1);
        }
        m_values.append(toData(GET_DATA, value, sensorId));
        return;
    }
}

void Simulator::sendAllValues(bool forced)
{
    QByteArray values;
    if(m_values.size() > MAX_VALUES){
        qWarning() << "More values than possible";
    }
    quint16 size = static_cast<quint16>(m_values.size());
    size >>= 1;
    qDebug() << "Sending a pack of " << size  << " values = " << size / 2  << " -> " << QString::number(size, 2) << " First = " << QString::number((size >> 8) & 0x00FF, 2);
    if(forced){
        values.append(GET_DATA);
    } else {
        values.append(SEND_MODE2_DATA);
    }

    values.append(char((size >> 8) & 0x00FF));
    values.append(char(size & 0x00FF));
    m_values.prepend(values);
    m_port.write(m_values);
    m_port.flush();
    m_values.clear();
}


QByteArray Simulator::success(qint8 command)
{
    QByteArray arr;
    arr.append(command);
    arr.append(SUCCESS_BIT);
    return arr;
}

QByteArray Simulator::failure(qint8 command)
{
    QByteArray arr;
    arr.append(command);
    arr.append(ERROR_BIT);
    return arr;
}

QByteArray Simulator::setCurrentMode(WORKING_MODE nwMode)
{
    qint8 modeInt = static_cast<qint8>(nwMode);
    if(nwMode == m_mode){
        return success(modeInt);
    }
    m_mode = nwMode;
    m_mode2Timer.stop();
    return success(modeInt);
}

QByteArray Simulator::getFrequencies()
{
    QByteArray res;
    res.append(STOP_MODE);
    res.append(m_sensor1.frequency());
    res.append(m_sensor2.frequency());
    qDebug() << m_sensor3.frequency();
    res.append(m_sensor3.frequency());
    quint8 mode2Freq = quint8(int(1.f / (m_mode2Timer.interval() / 1000.f)));
    res.append(mode2Freq);
    return res;
}

void Simulator::readCommand(const QByteArray &array)
{
    if(array.isEmpty()) return;
    qint8 first = array.at(0);
    qint8 data = array.size() > 1 ? array.at(1) : 0;
    qWarning() << "Received command " << QString::number(first);
    switch (first) {
    case STOP_MODE:
        if(!m_started){
            m_started = true;
            m_port.write(getFrequencies());
        } else {
            m_port.write(setCurrentMode(WORKING_MODE::NO_MODE));
        }
        break;
    case START_MODE_1:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_1));
        break;
    case START_MODE_2:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_2));
        m_mode2Timer.start();
        break;
    case START_MODE_3:
        m_port.write(setCurrentMode(WORKING_MODE::MODE_3));
        break;
    case GET_DATA:
        sendAllValues(true);
        return;
    case CONFIGURE_FE_1:
        m_sensor1.setEmitingSpeed(data);
        m_port.write(success(CONFIGURE_FE_1));
        break;
    case CONFIGURE_FE_2:
        m_sensor2.setEmitingSpeed(data);
        m_port.write(success(CONFIGURE_FE_2));
        break;
    case CONFIGURE_FE_3:
        m_sensor3.setEmitingSpeed(data);
        m_port.write(success(CONFIGURE_FE_3));
        break;
    case CONFIGURE_MODE_2:
        m_mode2Timer.setInterval(data);
        m_port.write(success(CONFIGURE_MODE_2));
        break;
    }
    m_port.flush();
}
