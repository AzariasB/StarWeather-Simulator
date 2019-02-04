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
    SEND_MODE2_DATA = 0xA,
    GET_FREQUENCIES = 0xB
};

/**
 * @brief The Simulator class
 * acts like the arduino
 * received commands and sends
 * data back
 */
class Simulator : public QObject
{
    Q_OBJECT
public:
    explicit Simulator(QSerialPort &port, QObject *parent = nullptr);


private:
    /**
     * @brief All the sensors of the simulator
     */
    Sensor m_sensor1;
    Sensor m_sensor2;
    Sensor m_sensor3;

    /**
     * @brief m_port the port used to communicate
     * with the desktop
     */
    QSerialPort &m_port;

    /**
     * @brief m_mode2Timer timer for the mode 2
     * to send data on a regular basis
     */
    QTimer m_mode2Timer;

    /**
     * @brief m_values sensed by the sensors
     * is filled up until send, then cleared
     */
    QByteArray m_values;

    /**
     * @brief m_started wether the simulator started
     */
    bool m_started = false;

    /**
     * @brief m_mode the current mode of the simulator
     */
    WORKING_MODE m_mode = WORKING_MODE::NO_MODE;

    /**
     * @brief readCommand when receiving data on the serialport
     * @param array data received
     */
    void readCommand(const QByteArray &array);

    /**
     * @brief receiveValue when a sensor generated a value, must store it on
     * the m_values byte array
     * @param value value generated
     * @param tmstp time at which it was generated
     * @param sensorId id of the sensor that generated the value
     */
    void receiveValue(qint16 value, quint32 tmstp, qint8 sensorId);

    /**
     * @brief toData turns the given data of a generated value into a byte array
     * @param sendingMode the mode used to send the value,
     * if the mode is 'direct' we need to add that to the byte array
     * otherwise, juste put the raw data
     * @param timestamp time at which the data was generated
     * @param value value generated
     * @param sensorId sensor that generated the value
     * @return the filled up byte array
     */
    inline QByteArray toData(qint8 sendingMode, quint32 timestamp, qint16 value, qint8 sensorId) const;

    /**
     * @brief sendAllValues sends all the values stored up until now
     * @param forced  true = mode 3 (asked by user), false = mode 2 (timer timed out)
     */
    void sendAllValues(bool forced);

    /**
     * @brief getFrequencies sends to the desktop all the frequencies of the sensors
     * @return byte array containing all the frequencies
     */
    QByteArray getFrequencies();

    /**
     * @brief setCurrentMode changes the working mode of the simulator
     * @param nwMode new mode to set
     * @return the confirmation code back, to be sent to the desktop
     */
    QByteArray setCurrentMode(WORKING_MODE nwMode);

    /**
     * In order to be as close as possible to the arduino, instead of sending full byte array,
     * we send values byte by byte, thus simulating the aruino perfectly
     * @brief sendByte sends a single byte
     * @param byte the byte to send
     * @return if the byte was successfully sent
     */
    bool sendByte(quint8 byte);

    /**
     * @brief sendBytes sends a byte array, byte by byte, to simulate the arduino
     * @param bytes all the bytes to send
     * @return if all the bytes where sent
     */
    bool sendBytes(const QByteArray &bytes);

    /**
     * @brief success returns the 'success' byte for the given command
     * @param command
     * @return
     */
    static QByteArray success(qint8 command);

    /**
     * @brief failure the 'error' message for the given command
     * @param command
     * @return
     */
    static QByteArray failure(qint8 command);

};

