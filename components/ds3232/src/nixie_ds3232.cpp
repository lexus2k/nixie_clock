/*
    Copyright (C) 2016-2018 Alexey Dynda

    This file is part of Nixie Library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nixie_ds3232.h"

bool Ds3231::begin()
{
    /* Init DS3232/DS3231 */
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x0E); // Write to control register
    m_i2c.write(0B00011100);
    m_i2c.write(0B00110000);
    if ( m_i2c.endTransmission() != 0)
    {
        m_no_device = true;
        m_seconds     = 0x00;
        m_minutes     = 0x00;
        m_hours       = 0x00;
        m_day         = 0x01;
        m_month       = 0x01;
        m_year        = 0x17;
        m_day_of_week = getDayOfWeek(toDecimal(m_year) + 2000, toDecimal(m_month), toDecimal(m_day)) + 1;
    }
    else
    {
        getDateTime();
    }
    return !m_no_device;
}

Ds3231& Ds3231::operator  ++() 
{
    if (++m_seconds > 59)
    {
        m_seconds -= 60;
        if (++m_minutes > 59)
        {
            m_minutes -= 60;
            if (++m_hours > 23) m_hours -= 24;
        }
    }
    return *this;
}


void Ds3231::getDateTime()
{
    if (m_no_device)
    {
        return;
    }
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x00);
    m_i2c.endTransmission();
    uint8_t buf[8];
    m_i2c.requestFrom(I2C_ADDR_DS3231, buf, 7);
  
    m_seconds     = buf[0];
    m_minutes     = buf[1];
    m_hours       = buf[2];
    m_day_of_week = buf[3];
    m_day         = buf[4];
    m_month       = buf[5] & 0x1F;
    m_year        = buf[6];
}

void Ds3231::setDateTime()
{
    if (m_no_device)
    {
        return;
    }
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x00);
    m_i2c.write(m_seconds);
    m_i2c.write(m_minutes);
    m_i2c.write(m_hours);
    m_i2c.write(m_day_of_week);
    m_i2c.write(m_day);
    m_i2c.write(m_month);
    m_i2c.write(m_year);
    m_i2c.endTransmission();
}

void    Ds3231::setDate()
{
    if (m_no_device)
    {
        return;
    }
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x03);
    m_i2c.write(m_day_of_week);
    m_i2c.write(m_day);
    m_i2c.write(m_month);
    m_i2c.write(m_year);
    m_i2c.endTransmission();
}

void    Ds3231::setTime()
{
    if (m_no_device)
    {
        return;
    }
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x00);
    m_i2c.write(m_seconds);
    m_i2c.write(m_minutes);
    m_i2c.write(m_hours);
    m_i2c.endTransmission();
}

int16_t Ds3231::getTemp()
{
    if (m_no_device)
    {
        return 0;
    }
    //temp registers (11h-12h) get updated automatically every 64s
    m_i2c.beginTransmission(I2C_ADDR_DS3231);
    m_i2c.write(0x11);
    m_i2c.endTransmission();
    uint8_t buf[2];
    m_i2c.requestFrom(I2C_ADDR_DS3231, buf, 2);
    uint8_t temp = buf[0];
    return (((temp & 0x7F) << 2) | (buf[1] >> 6)) * ((temp & 0x80) ? -1: 1);
}

uint8_t Ds3231::toInternal(uint8_t d)
{
    if (d > 127)
    {
        return 0xFF;
    }
    return ((d / 10) << 4) | (d % 10);
}

uint8_t Ds3231::toDecimal(uint8_t i)
{
    return (i & 0x0F) + (i>>4) * 10;
};

int16_t Ds3231::toMinutes(uint8_t hours, uint8_t minutes, bool decimal)
{
    if (!decimal)
    {
         hours = toDecimal(hours);
         minutes = toDecimal(minutes);
    }
    return ((int16_t)hours)*60 + (int16_t)minutes;
}

int16_t Ds3231::timeDelta(int16_t min1, int16_t min2)
{
    int16_t delta1 = min2 - min1;
    int16_t delta2;
    if (delta1 < 0)
    {
        delta1 = - delta1;
    }
    delta2 = 24*60 - delta1;
    return delta1 < delta2 ? delta1: delta2;
}

uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t a = (14 - month) / 12;
    uint16_t y = year - a;
    uint8_t m = month + 12 * a - 2;
    uint8_t dayOfWeek = (7000 + day + y + y / 4 - y / 100 + y / 400 + (31 * (uint16_t)m) / 12) % 7;
    return dayOfWeek;
}





