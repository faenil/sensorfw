/**
   @file gyroscopeadaptor.cpp
   @brief GyroscopeAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Pia Niemelä <pia.s.niemela@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/
#include <errno.h>

#include "logging.h"
#include "config.h"
#include "datatypes/utils.h"

#include "gyroscopeadaptor.h"

GyroscopeAdaptor::GyroscopeAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode)
{

    QString path = Config::configuration()->value("gyroscope_sysfs_path").toString();
    if ( !addPath(path, 0) ) {
        setValid(false);
    }

    gyroscopeBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(32);
    setAdaptedSensor("gyroscope", "l3g4200dh", gyroscopeBuffer_);

    introduceAvailableDataRange(DataRange(-250000, 250000, 1));
    setDescription("Sysfs Gyroscope adaptor (l3g4200dh)");

    introduceAvailableInterval(DataRange(100, 100, 1));     // 10 Hz
    introduceAvailableInterval(DataRange(25, 25, 1));       // 40 Hz
    introduceAvailableInterval(DataRange(10, 10, 1));       // 100 Hz
    introduceAvailableInterval(DataRange(5, 5, 1));         // 200 Hz
    introduceAvailableInterval(DataRange(2.5, 2.5, 1));     // 400 Hz
    introduceAvailableInterval(DataRange(1.25, 1.25, 1));   // 800 Hz
    setDefaultInterval(10); // 100 Hz

}

GyroscopeAdaptor::~GyroscopeAdaptor()
{
    delete gyroscopeBuffer_;
}

void GyroscopeAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);
    short x, y, z;
    char buf[32];

    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    sensordLogT() << "gyroscope output value: " << buf;

    sscanf(buf, "%hd %hd %hd\n", &x, &y, &z);

    TimedXyzData* pos = gyroscopeBuffer_->nextSlot();
    pos->x_ = x;
    pos->y_ = y;
    pos->z_ = z;
    pos->timestamp_ = Utils::getTimeStamp();

    gyroscopeBuffer_->commit();
    gyroscopeBuffer_->wakeUpReaders();
}


bool GyroscopeAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    return SysfsAdaptor::setInterval(value>10?10:value, sessionId);
}

unsigned int GyroscopeAdaptor::interval() const{
    return SysfsAdaptor::interval();
}



