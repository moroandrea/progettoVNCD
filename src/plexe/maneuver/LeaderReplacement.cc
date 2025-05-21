#include "plexe/maneuver/LeaderReplacement.h"

namespace plexe {

LeaderReplacement::LeaderReplacement(BasePlatooningApp* app)
    : BaseManeuver(app)
    , leaderReplacementState(LeaderReplacementState::IDLE)
{
}

void LeaderReplacement::startManeuver(const void* parameters)
{
    // TODO
}

void LeaderReplacement::abortManeuver()
{
    leaderReplacementState = LeaderReplacementState::IDLE;
}

void LeaderReplacement::onManeuverMessage(const ManeuverMessage* mm)
{
    if (const LeaderAvailabilityRequest* msg = dynamic_cast<const LeaderAvailabilityRequest*>(mm)) {
        getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAvailabilityRequest!!!");
        handleLeaderAvailabilityRequest(msg);
    }
    else if (const LeaderAvailabilityResponse* msg = dynamic_cast<const LeaderAvailabilityResponse*>(mm)) {
        getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAvailabilityResponse!!!");
        handleLeaderAvailabilityResponse(msg);
    }
    else if (const LeaderAbandonIntention* msg = dynamic_cast<const LeaderAbandonIntention*>(mm)) {
        getSimulation()->getActiveEnvir()->alert("E' arrivato un LeaderAbandonIntention!!!");
        handleLeaderAbandonIntention(msg);
    }
    else if (const ReadyToBecomeLeader* msg = dynamic_cast<const ReadyToBecomeLeader*>(mm)) {
        getSimulation()->getActiveEnvir()->alert("E' arrivato un ReadyToBecomeLeader!!!");
        handleReadyToBecomeLeader(msg);
    }
    else if (const UpdatePlatoonFormation* msg = dynamic_cast<const UpdatePlatoonFormation*>(mm)) {
        getSimulation()->getActiveEnvir()->alert("E' arrivato un UpdatePlatoonFormation!!!");
        handleUpdatePlatoonFormation(msg);
    }
}

void LeaderReplacement::handleLeaderAvailabilityRequest(const LeaderAvailabilityRequest* msg)
{
    // TODO
}

void LeaderReplacement::handleLeaderAvailabilityResponse(const LeaderAvailabilityResponse* msg)
{
    // TODO
}

void LeaderReplacement::handleLeaderAbandonIntention(const LeaderAbandonIntention* msg)
{
    // TODO
}

void LeaderReplacement::handleReadyToBecomeLeader(const ReadyToBecomeLeader* msg)
{
    // TODO
}

void LeaderReplacement::handleUpdatePlatoonFormation(const UpdatePlatoonFormation* msg)
{
    // TODO
}

void LeaderReplacement::sendLeaderAvailabilityRequest()
{
    LeaderAvailabilityRequest* msg = createLeaderAvailabilityRequest();
    int dest = positionHelper->getMemberId(1);
    msg->setDestinationId(dest);
    app->sendUnicast(msg, dest);
}

void LeaderReplacement::sendLeaderAvailabilityResponse()
{
    LeaderAvailabilityResponse* msg = createLeaderAvailabilityResponse();
    int dest = positionHelper->getLeaderId();
    msg->setDestinationId(dest);
    app->sendUnicast(msg, dest);
}

void LeaderReplacement::sendLeaderAbandonIntention()
{
    LeaderAbandonIntention* msg = createLeaderAbandonIntention();

    for (unsigned int i = 1; i < positionHelper->getPlatoonSize(); i++) {
        int dest = positionHelper->getMemberId(i);
        LeaderAbandonIntention* dup = msg->dup();
        dup->setDestinationId(dest);
        app->sendUnicast(dup, dest);
    }
}

void LeaderReplacement::sendReadyToBecomeLeader()
{
    ReadyToBecomeLeader* msg = createReadyToBecomeLeader();
    int dest = positionHelper->getLeaderId();
    msg->setDestinationId(dest);
    app->sendUnicast(msg, dest);
}

void LeaderReplacement::broadcastUpdatePlatoonFormation(std::vector<int>& formation)
{
    UpdatePlatoonFormation* msg = createUpdatePlatoonFormation(formation);

    for (unsigned int i = 1; i < formation.size(); i++) {
        int dest = formation[i];
        UpdatePlatoonFormation* dup = msg->dup();
        dup->setDestinationId(dest);
        app->sendUnicast(dup, dest);
    }
}

void LeaderReplacement::fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId)
{
    msg->setKind(MANEUVER_TYPE);
    msg->setVehicleId(vehicleId);
    msg->setExternalId(externalId.c_str());
    msg->setPlatoonId(platoonId);
}

LeaderAvailabilityRequest* LeaderReplacement::createLeaderAvailabilityRequest()
{
    LeaderAvailabilityRequest* msg = new LeaderAvailabilityRequest("LeaderAvailabilityRequest");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

LeaderAvailabilityResponse* LeaderReplacement::createLeaderAvailabilityResponse()
{
    LeaderAvailabilityResponse* msg = new LeaderAvailabilityResponse("LeaderAvailabilityResponse");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

LeaderAbandonIntention* LeaderReplacement::createLeaderAbandonIntention()
{
    LeaderAbandonIntention* msg = new LeaderAbandonIntention("LeaderAbandonIntention");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

ReadyToBecomeLeader* LeaderReplacement::createReadyToBecomeLeader()
{
    ReadyToBecomeLeader* msg = new ReadyToBecomeLeader("ReadyToBecomeLeader");
    fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    return msg;
}

UpdatePlatoonFormation* LeaderReplacement::createUpdatePlatoonFormation(const std::vector<int>& formation)
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

} // namespace plexe
