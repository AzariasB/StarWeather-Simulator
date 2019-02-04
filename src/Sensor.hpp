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
 * File:   Sensor.hpp
 * Author: azarias
 *
 * Created on 31/12/2018
 */
#pragma once

#include <QObject>
#include <QTimer>

#define TEN_BITS 0b0000001111111111

/**
 * @brief The Sensor class
 * Simulate an arduino sensor
 * emit a signal whenever the timer ends
 * it is possible to configure the emition rate
 */
class Sensor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Sensor constructor
     * @param timer timeout
     * @param parent
     */
    explicit Sensor(int timer = 20, QObject *parent = nullptr);

    /**
     * @brief frequency frequency at which this sensor emits
     * @return
     */
    quint8 frequency() const;

    /**
     * @brief setEmitingSpeed changes the emiting speed
     * @param frequency
     */
    void setEmitingSpeed(qint8 frequency);

    /**
     * @brief restart restarts the timer
     * and sets the timestamp to 0
     */
    void restart();

signals:
    /**
     * @brief sensedValue signal emitted when this sensor generates a value
     * @param value the value generated
     * @param timestamp the time when the value was generated
     */
    void sensedValue(qint16 value, quint32 timestamp);

private:
    QTimer m_timer;

    quint16 m_value;

    quint32 m_timestamp = 0;

    /**
     * @brief create an emit a fake value (encoded on ten bits)
     */
    inline void fakeValue();
};
