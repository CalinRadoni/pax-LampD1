/**
This file is part of pax-LampD1 (https://github.com/CalinRadoni/pax-LampD1)
Copyright (C) 2019 by Calin Radoni

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

#ifndef HTTPSrvLampD1_H
#define HTTPSrvLampD1_H

#include "pax_http_server.h"

class HTTPSrvLampD1 : public PaxHttpServer
{
public:
    HTTPSrvLampD1(void);
    virtual ~HTTPSrvLampD1();

    /**
     * Data for status string
     */
    uint32_t animationID = 0;
    uint32_t currentColor = 0x010101;
    uint32_t currentIntensity = 0;

protected:
    /**
     * @warning Delete returned string with 'free' !
     */
    virtual char* CreateJSONStatusString(bool addWhitespaces);
};

#endif
