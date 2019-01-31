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

Sensor::Sensor(int timer, QObject *parent) : QObject(parent),
  m_timer()
{
    m_timer.setSingleShot(false);
    m_timer.start(timer);
    connect(&m_timer, &QTimer::timeout, [&](){ fakeValue(); });
}

void Sensor::setEmitingSpeed(qint8 milliseconds)
{
    m_timer.setInterval(milliseconds);
    m_timer.start();
}

inline void Sensor::fakeValue()
{
    qint16 rand = static_cast<qint16>(qrand() & TEN_BITS);
    qint8 frequency = static_cast<qint8>(1 / (m_timer.interval() / 1000.f));
    emit sensedValue(rand, frequency);
}
