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
 * File:   Sensor.cpp
 * Author: azarias
 *
 * Created on 31/12/2018
 */
#include "Sensor.hpp"
#include "Frequency.hpp"

Sensor::Sensor(int timer, QObject *parent) : QObject(parent),
  m_timer()
{
    m_value = static_cast<qint16>(qrand() & TEN_BITS);
    m_timer.setSingleShot(false);
    m_timer.start(timer);
    connect(&m_timer, &QTimer::timeout, [&](){ fakeValue(); });
}

void Sensor::setEmitingSpeed(qint8 frequency)
{
    m_timer.setInterval(toMilliseconds<int>(frequency));
    m_timer.start();
}

quint8 Sensor::frequency() const
{
    return toFrequency<quint8>(m_timer.interval());
}

void Sensor::restart()
{
    m_timestamp = 0;
    m_timer.start();
}

inline void Sensor::fakeValue()
{
    m_value = std::clamp<quint16>( (m_value + (qrand() % 60) - 30) & TEN_BITS , 0, 1024);
    emit sensedValue(m_value, m_timestamp);
    m_timestamp += m_timer.interval();
}
