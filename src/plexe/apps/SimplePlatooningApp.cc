//
// Copyright (C) 2012-2023 Michele Segata <segata@ccs-labs.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "plexe/apps/SimplePlatooningApp.h"
#include "plexe/messages/PlexeInterfaceControlInfo_m.h"
#include "plexe/protocols/BaseProtocol.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/utility/Consts80211p.h"

using namespace veins;

namespace plexe {

Define_Module(SimplePlatooningApp);

void SimplePlatooningApp::initialize(int stage)
{
    BaseApp::initialize(stage);

    if (stage == 1) {
        // connect maneuver application to protocol
        protocol->registerApplication(
            MANEUVER_TYPE,
            gate("lowerLayerIn"), gate("lowerLayerOut"),
            gate("lowerControlIn"), gate("lowerControlOut")
        );
        // register to the signal indicating failed unicast transmissions
        findHost()->subscribe(Mac1609_4::sigRetriesExceeded, this);
    }
}

void SimplePlatooningApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(msg);

    cPacket* enc = frame->getEncapsulatedPacket();
    ASSERT2(enc, "received a BaseFrame1609_4s with nothing inside");

    if (enc->getKind() == MANEUVER_TYPE) {
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(frame->decapsulate());
        if (LeaderAbandonIntention* msg = dynamic_cast<LeaderAbandonIntention*>(mm)) {
            //handleUpdatePlatoonData(msg);
            getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAbandonMessage!!!");
            delete msg;
        }
        else if (LeaderAvailabilityRequest* msg = dynamic_cast<LeaderAvailabilityRequest*>(mm)) {
                    //handleLeaderAvailabilityRequest(msg);
                    getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAvailabilityRequest!!!");
                    delete msg;
        }
        else if (LeaderAvailabilityResponse* msg = dynamic_cast<LeaderAvailabilityResponse*>(mm)) {
                    //handleLeaderAvailabilityResponse(msg);
                    getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAvailabilityResponse!!!");
                    delete msg;
        }
        else if (ReadyToBecomeLeader* msg = dynamic_cast<ReadyToBecomeLeader*>(mm)) {
                    //handleReadyToBecomeLeader(msg);
                    getSimulation()->getActiveEnvir()->alert("E' arrivato un leaderAbandonMessage!!!");
                    delete msg;
        }
        else if (UpdatePlatoonFormation* msg = dynamic_cast<UpdatePlatoonFormation*>(mm)) {
            //handleUpdatePlatoonFormation(msg);
            getSimulation()->getActiveEnvir()->alert("E' arrivato un leaderAbandonMessage!!!");
            delete msg;
        }
        delete frame;
    }
    else {
        BaseApp::handleLowerMsg(msg);
    }
}

void SimplePlatooningApp::sendUnicast(cPacket* msg, int destination)
{
    Enter_Method_Silent();
    take(msg);

    // Initialize base frame
    BaseFrame1609_4* frame = new BaseFrame1609_4("BaseFrame1609_4", msg->getKind());
    frame->setRecipientAddress(destination);
    frame->setChannelNumber(static_cast<int>(Channel::cch));
    frame->encapsulate(msg);

    // send unicast frames using 11p only
    PlexeInterfaceControlInfo* ctrl = new PlexeInterfaceControlInfo();
    ctrl->setInterfaces(PlexeRadioInterfaces::VEINS_11P);
    frame->setControlInfo(ctrl);
    sendDown(frame);
}

void SimplePlatooningApp::sendLeaderAvailabilityRequest()
{
    LeaderAvailabilityRequest* msg = createLeaderAvailabilityRequestMsg();
    int dest = positionHelper->getMemberId(1);
    msg->setDestinationId(dest);
    sendUnicast(msg, dest);
}

void SimplePlatooningApp::sendLeaderAvailabilityResponse()
{
    LeaderAvailabilityResponse* msg = createLeaderAvailabilityResponseMsg();
    int dest = positionHelper->getLeaderId();
    msg->setDestinationId(dest);
    sendUnicast(msg, dest);
}

void SimplePlatooningApp::sendLeaderAbandonIntention()
{
    LeaderAbandonIntention* msg = createLeaderAbandonIntentionMsg();

    for (unsigned int i = 1; i < positionHelper->getPlatoonSize(); i++) {
        int dest = positionHelper->getMemberId(i);
        LeaderAbandonIntention* dup = msg->dup();
        dup->setDestinationId(dest);
        sendUnicast(dup, dest);
    }
}

void SimplePlatooningApp::sendReadyToBecomeLeader()
{
    ReadyToBecomeLeader* msg = createReadyToBecomeLeaderMsg();
    int dest = positionHelper->getLeaderId();
    msg->setDestinationId(dest);
    sendUnicast(msg, dest);
}

void SimplePlatooningApp::broadcastUpdateFormation(std::vector<int>& formation)
{
    UpdatePlatoonFormation* msg = createUpdateFormationMsg(formation);

    for (unsigned int i = 1; i < formation.size(); i++) {
        int dest = formation[i];
        UpdatePlatoonFormation* dup = msg->dup();
        dup->setDestinationId(dest);
        sendUnicast(dup, dest);
    }
}

void SimplePlatooningApp::handleLeaderAvailabilityRequest(const LeaderAvailabilityRequest* msg)
{

}

void SimplePlatooningApp::handleLeaderAvailabilityResponse(const LeaderAvailabilityResponse* msg)
{

}

void SimplePlatooningApp::handleLeaderAbandonIntention(const LeaderAbandonIntention* msg)
{

}

void SimplePlatooningApp::handleReadyToBecomeLeader(const ReadyToBecomeLeader* msg)
{

}

void SimplePlatooningApp::handleUpdateFormation(const UpdatePlatoonFormation* msg)
{

}

void SimplePlatooningApp::fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId)
{
    msg->setKind(MANEUVER_TYPE);
    msg->setVehicleId(vehicleId);
    msg->setExternalId(externalId.c_str());
    msg->setPlatoonId(platoonId);
}

LeaderAvailabilityRequest* SimplePlatooningApp::createLeaderAvailabilityRequestMsg()
{
    LeaderAvailabilityRequest* msg = new LeaderAvailabilityRequest("LeaderAvailabilityRequest");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

LeaderAvailabilityResponse* SimplePlatooningApp::createLeaderAvailabilityResponseMsg()
{
    LeaderAvailabilityResponse* msg = new LeaderAvailabilityResponse("LeaderAvailabilityResponse");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

LeaderAbandonIntention* SimplePlatooningApp::createLeaderAbandonIntentionMsg()
{
    LeaderAbandonIntention* msg = new LeaderAbandonIntention("LeaderAbandonIntention");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

ReadyToBecomeLeader* SimplePlatooningApp::createReadyToBecomeLeaderMsg()
{
    ReadyToBecomeLeader* msg = new ReadyToBecomeLeader("ReadyToBecomeLeader");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

UpdatePlatoonFormation* SimplePlatooningApp::createUpdateFormationMsg(const std::vector<int>& formation)
{
    UpdatePlatoonFormation* msg = new UpdatePlatoonFormation("UpdatePlatoonFormation");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());

    msg->setPlatoonSpeed(positionHelper->getPlatoonSpeed());
    msg->setPlatoonLane(positionHelper->getPlatoonLane());

    msg->setPlatoonFormationArraySize(formation.size());
    for (unsigned int i = 0; i < formation.size(); i++) {
        msg->setPlatoonFormation(i, formation[i]);
    }

    return msg;
}

void SimplePlatooningApp::setPlatoonRole(PlatoonRole r)
{
    role = r;
}

} // namespace plexe
