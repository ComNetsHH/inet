//
// Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
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

#ifndef __INET_ETHERAPPCLI_H
#define __INET_ETHERAPPCLI_H

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Packet.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/ieee8022/LlcSocket.h"
#include "inet/linklayer/ieee8022/LlcSocketCommand_m.h"

namespace inet {

/**
 * Simple traffic generator for the Ethernet model.
 */
class INET_API EtherAppClient : public cSimpleModule, public ILifecycle, public LlcSocket::ICallback
{
  protected:
    enum Kinds { START = 100, NEXT };

    // send parameters
    long seqNum = 0;
    cPar *reqLength = nullptr;
    cPar *respLength = nullptr;
    cPar *sendInterval = nullptr;

    int localSAP = -1;
    int remoteSAP = -1;
    MacAddress destMACAddress;
    NodeStatus *nodeStatus = nullptr;

    // self messages
    cMessage *timerMsg = nullptr;
    simtime_t startTime;
    simtime_t stopTime;

    LlcSocket llcSocket;

    // receive statistics
    long packetsSent = 0;
    long packetsReceived = 0;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    virtual bool isNodeUp();
    virtual bool isGenerator();
    virtual void scheduleNextPacket(bool start);
    virtual void cancelNextPacket();

    virtual MacAddress resolveDestMACAddress();

    virtual void sendPacket();
    virtual void socketDataArrived(LlcSocket*, Packet *msg) override;
    virtual void registerDSAP(int dsap);
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

  public:
    EtherAppClient() {}
    virtual ~EtherAppClient();
};

} // namespace inet

#endif // ifndef __INET_ETHERAPPCLI_H

