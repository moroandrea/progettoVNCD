#include "plexe/maneuver/LeaderReplacement.h"
#include "plexe/apps/BasePlatooningApp.h"

namespace plexe {

LeaderReplacement::LeaderReplacement(BasePlatooningApp* app)
    : BaseManeuver(app)
    , leaderReplacementState(LeaderReplacementState::IDLE)
    , isCandidate(false)
    , candidateId(-1)
{
}

bool LeaderReplacement::initializeReplacementManeuver(const void* parameters)
{
    if (leaderReplacementState == LeaderReplacementState::IDLE) {
        if (app->isInManeuver()) {
            LOG << positionHelper->getId() << " cannot begin the maneuver because already involved in another one\n";
            return false;
        }

        app->setInManeuver(true, this);

        return true;
    }
    else {
        return false;
    }
}

void LeaderReplacement::startManeuver(const void* parameters)
{
    if (initializeReplacementManeuver(parameters)) {
        // Send leader request to vehicle immediately behind
        int destinationId = positionHelper->getMemberId(1);
        sendLeaderAvailabilityRequest(destinationId);

        candidateId = destinationId;
        leaderReplacementState = LeaderReplacementState::WAIT_AVAILABILITY_LEADER;
    }
}

void LeaderReplacement::abortManeuver()
{
    leaderReplacementState = LeaderReplacementState::IDLE;
    app->setInManeuver(false, nullptr);
}

void LeaderReplacement::onManeuverMessage(const ManeuverMessage* mm)
{
    std::ostringstream alertMsg;

    // Common metadata
    std::string sender = std::to_string(mm->getVehicleId());
    std::string receiver = std::to_string(positionHelper->getId()); // Our own ID
    std::string platoonId = std::to_string(mm->getPlatoonId());

    if (const LeaderAvailabilityRequest* msg = dynamic_cast<const LeaderAvailabilityRequest*>(mm)) {
        alertMsg << "[LeaderAvailabilityRequest] From: " << sender
                 << ", To: " << receiver
                 << ", Platoon: " << platoonId;
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());
        handleLeaderAvailabilityRequest(msg);
    }
    else if (const LeaderAvailabilityResponse* msg = dynamic_cast<const LeaderAvailabilityResponse*>(mm)) {
        alertMsg << "[LeaderAvailabilityResponse] From: " << sender
                 << ", To: " << receiver
                 << ", Platoon: " << platoonId;
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());
        handleLeaderAvailabilityResponse(msg);
    }
    else if (const LeaderAbandonIntention* msg = dynamic_cast<const LeaderAbandonIntention*>(mm)) {
        alertMsg << "[LeaderAbandonIntention] From: " << sender
                 << ", To: " << receiver
                 << ", Platoon: " << platoonId;
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());
        handleLeaderAbandonIntention(msg);
    }
    else if (const ReadyToBecomeLeader* msg = dynamic_cast<const ReadyToBecomeLeader*>(mm)) {
        alertMsg << "[ReadyToBecomeLeader] From: " << sender
                 << ", To: " << receiver
                 << ", Platoon: " << platoonId;
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());
        handleReadyToBecomeLeader(msg);
    }
    else if (const UpdatePlatoonFormation* msg = dynamic_cast<const UpdatePlatoonFormation*>(mm)) {
        alertMsg << "[UpdatePlatoonFormation] From: " << sender
                 << ", To: " << receiver
                 << ", Platoon: " << platoonId;
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());
        handleUpdatePlatoonFormation(msg);
    }
}

void LeaderReplacement::handleLeaderAvailabilityRequest(const LeaderAvailabilityRequest* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (leaderReplacementState != LeaderReplacementState::IDLE) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getDestinationId() != positionHelper->getId()) return;

    // For simplicity, the candidate always accepts the request
    isCandidate = true;
    sendLeaderAvailabilityResponse(true);
}

void LeaderReplacement::handleLeaderAvailabilityResponse(const LeaderAvailabilityResponse* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::LEADER) return;
    if (leaderReplacementState != LeaderReplacementState::WAIT_AVAILABILITY_LEADER) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != candidateId) return;

    if (msg->getAvailable() == true) {
        sendLeaderAbandonIntention();
        leaderReplacementState = LeaderReplacementState::WAIT_READY_TO_BECOME_LEADER;
    }
}

void LeaderReplacement::handleLeaderAbandonIntention(const LeaderAbandonIntention* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != positionHelper->getLeaderId()) return;

    leaderReplacementState = LeaderReplacementState::WAIT_FORMATION_UPDATE;
    app->setInManeuver(true, this);

    if (isCandidate == true) {
        sendReadyToBecomeLeader();
    }
}

void LeaderReplacement::handleReadyToBecomeLeader(const ReadyToBecomeLeader* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::LEADER) return;
    if (leaderReplacementState != LeaderReplacementState::WAIT_READY_TO_BECOME_LEADER) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != candidateId) return;

    // Remove itself from the platoon, and send the updated information
    std::vector<int> formation = positionHelper->getPlatoonFormation();
    formation.erase(formation.begin());
    broadcastUpdatePlatoonFormation(formation);

    leaderReplacementState = LeaderReplacementState::IDLE;
    app->setInManeuver(false, nullptr);

    // Update own platoon information
    positionHelper->setPlatoonId(-1);
    positionHelper->setPlatoonFormation(std::vector<int>());
    app->setPlatoonRole(PlatoonRole::NONE);

    // Turn on ACC
    plexeTraciVehicle->setActiveController(ACC);
    plexeTraciVehicle->setACCHeadwayTime(1.2);
}

void LeaderReplacement::handleUpdatePlatoonFormation(const UpdatePlatoonFormation* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != positionHelper->getLeaderId()) return;

    // update formation information
    LOG << positionHelper->getId() << " changing platoon formation: ";
    std::vector<int> f;
    for (unsigned int i = 0; i < msg->getPlatoonFormationArraySize(); i++) {
        f.push_back(msg->getPlatoonFormation(i));
        LOG << msg->getPlatoonFormation(i) << " ";
    }
    LOG << "\n";
    //Clear SUMO formation for resetting...
    cModule *traffic = findModuleByPath("<root>.traffic");
    std::string platooningVType = traffic->par("platooningVType");
    std::stringstream ss;
    std::vector<int> oldFormation = positionHelper->getPlatoonFormation();
    for (int i = 0; i< oldFormation.size(); i++) {
        int removeId = oldFormation[i];
        ss.clear();
        ss << platooningVType << "." << removeId;
        positionHelper->getExternalId();
        plexeTraciVehicle->removePlatoonMember(ss.str());
    }

    positionHelper->setPlatoonFormation(f);

    leaderReplacementState = LeaderReplacementState::IDLE;
    app->setInManeuver(false, nullptr);

    if (isCandidate == true) {
        app->setPlatoonRole(PlatoonRole::LEADER);
        isCandidate = false;
    }
}

void LeaderReplacement::sendLeaderAvailabilityRequest(int destinationId)
{
    LeaderAvailabilityRequest* msg = createLeaderAvailabilityRequest();
    msg->setDestinationId(destinationId);
    app->sendUnicast(msg, destinationId);
}

void LeaderReplacement::sendLeaderAvailabilityResponse(bool available)
{
    LeaderAvailabilityResponse* msg = createLeaderAvailabilityResponse();
    int dest = positionHelper->getLeaderId();
    msg->setDestinationId(dest);
    msg->setAvailable(available);
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

    std::vector<int> currentFormation = positionHelper->getPlatoonFormation();
    for (unsigned int i = 1; i < currentFormation.size(); i++) {
        int dest = currentFormation[i];
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
