#include "plexe/apps/BasePlatooningApp.h"

#include "plexe/messages/PlexeInterfaceControlInfo_m.h"
#include "plexe/protocols/BaseProtocol.h"

#include "veins/base/utils/FindModule.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/utility/Consts80211p.h"

using namespace veins;

namespace plexe {

Define_Module(BasePlatooningApp);

void BasePlatooningApp::initialize(int stage)
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

        // Set current platoon role
        if (positionHelper->isLeader()) setPlatoonRole(PlatoonRole::LEADER);
        else setPlatoonRole(PlatoonRole::FOLLOWER);

        // Retrieve current scenario
        scenario = FindModule<BaseScenario*>::findSubModule(getParentModule());
    }
}

void BasePlatooningApp::setPlatoonRole(PlatoonRole r)
{
    role = r;
}

void BasePlatooningApp::sendUnicast(cPacket* msg, int destination)
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

void BasePlatooningApp::handleLowerMsg(cMessage* msg)
{
    BaseApp::handleLowerMsg(msg);
}

void BasePlatooningApp::onManeuverMessage(ManeuverMessage* mm)
{
    if (activeManeuver) {
        activeManeuver->onManeuverMessage(mm);
    }
    delete mm;
}

} // namespace plexe
