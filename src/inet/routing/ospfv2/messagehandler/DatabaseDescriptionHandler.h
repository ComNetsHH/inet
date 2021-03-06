//
// Copyright (C) 2006 Andras Babos and Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_DATABASEDESCRIPTIONHANDLER_H
#define __INET_DATABASEDESCRIPTIONHANDLER_H

#include "inet/common/INETDefs.h"
#include "inet/routing/ospfv2/messagehandler/IMessageHandler.h"

namespace inet {

namespace ospfv2 {

class INET_API DatabaseDescriptionHandler : public IMessageHandler
{
  private:
    bool processDDPacket(const Ospfv2DatabaseDescriptionPacket *ddPacket, Ospfv2Interface *intf, Neighbor *neighbor, bool inExchangeStart);

  public:
    DatabaseDescriptionHandler(Router *containingRouter);

    void processPacket(Packet *packet, Ospfv2Interface *intf, Neighbor *neighbor) override;
};

} // namespace ospfv2

} // namespace inet

#endif // ifndef __INET_DATABASEDESCRIPTIONHANDLER_H

