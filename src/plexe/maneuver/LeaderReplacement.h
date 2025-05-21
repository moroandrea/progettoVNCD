#ifndef LEADERREPLACEMENT_H_
#define LEADERREPLACEMENT_H_

#include "plexe/maneuver/BaseManeuver.h"

#include "plexe/messages/LeaderAvailabilityRequest_m.h"
#include "plexe/messages/LeaderAvailabilityResponse_m.h"
#include "plexe/messages/LeaderAbandonIntention_m.h"
#include "plexe/messages/ReadyToBecomeLeader_m.h"
#include "plexe/messages/UpdatePlatoonFormation_m.h"

using namespace veins;

namespace plexe {

class LeaderReplacement : public BaseManeuver {

public:
    /**
     * Constructor
     *
     * @param app pointer to the base platooning application used to fetch parameters and inform it about a concluded maneuver
     */
    LeaderReplacement(BasePlatooningApp* app);
    virtual ~LeaderReplacement(){};

    /**
     * This method is invoked by the base platooning application to start the maneuver
     *
     * @param parameters parameters passed to the maneuver
     */
    virtual void startManeuver(const void* parameters) override;

    /**
     * Handles the abortion of the maneuver when required by the base platooning application.
     * This method does currently nothing and it is meant for future used and improved maneuvers.
     */
    virtual void abortManeuver() override;

    /**
     * Handles the incoming maneuver message, if pertinent to the maneuver.
     *
     * @param mm maneuver message
     */
    virtual void onManeuverMessage(const ManeuverMessage* mm) override;

    /**
     * Handles the reception of a LeaderAvailabilityRequest message.
     *
     * @param LeaderAvailabilityRequest msg The leader abandon intention message.
     */
    void handleLeaderAvailabilityRequest(const LeaderAvailabilityRequest* msg);

    /**
     * Handles the reception of a LeaderAvailabilityResponse message.
     *
     * @param LeaderAvailabilityResponse msg The leader abandon intention message.
     */
    void handleLeaderAvailabilityResponse(const LeaderAvailabilityResponse* msg);

    /**
     * Handles the reception of a LeaderAbandonIntention message.
     *
     * @param LeaderAbandonIntention msg The leader abandon intention message.
     */
    void handleLeaderAbandonIntention(const LeaderAbandonIntention* msg);

    /**
     * Handles the reception of a ReadyToBecomeLeader message.
     *
     * @param ReadyToBecomeLeader msg The ready-to-become leader message.
     */
    void handleReadyToBecomeLeader(const ReadyToBecomeLeader* msg);

    /**
     * Handles the reception of a UpdatePlatoonFormation message.
     *
     * @param UpdatePlatoonFormation msg The updated platoon formation message.
     */
    void handleUpdatePlatoonFormation(const UpdatePlatoonFormation* msg);

protected:
    enum class LeaderReplacementState {
        IDLE, ///< The maneuver did not start
    };

    /**
     * Sends a message to a vehicle in the platoon, asking if it's available
     * to become the new platoon leader.
     */
    void sendLeaderAvailabilityRequest();

    /**
     * Sends a message to the platoon leader, confirming or denying its availability
     * to become the new platoon leader.
     */
    void sendLeaderAvailabilityResponse();

    /**
     * Sends a message informing all vehicles that it's about to leave the platoon.
     */
    void sendLeaderAbandonIntention();

    /**
     * Sends a message to the platoon leader, informing that it's ready to take
     * its place as platoon leader.
     */
    void sendReadyToBecomeLeader();

    /**
     * Sends the updated platoon formation to all other platoon
     * members.
     *
     * @param std::vector<int> formation The platoon formation.
     */
    void broadcastUpdatePlatoonFormation(std::vector<int>& formation);

    /**
     * Fills the ManeuverMessage with all the basic information.
     *
     * @param msg ManeuverMessage The message to be filled
     * @param int vehicleId The id of the sending vehicle
     * @param int platoonId The id of the platoon of the sending vehicle
     * @param int destinationId The id of the destination
     */
    void fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId);

    LeaderReplacementState leaderReplacementState;

private:
    /**
     * Creates a LeaderAvailabilityRequest message.
     */
    LeaderAvailabilityRequest* createLeaderAvailabilityRequest();

    /**
     * Creates a LeaderAvailabilityResponse message.
     */
    LeaderAvailabilityResponse* createLeaderAvailabilityResponse();

    /**
     * Creates a LeadearAbandonIntention message.
     */
    LeaderAbandonIntention* createLeaderAbandonIntention();

    /**
     * Creates a ReadyToBecomeLeader message.
     */
    ReadyToBecomeLeader* createReadyToBecomeLeader();

    /**
     * Creates a UpdatePlatoonFormation message, using the specified platoon formation.
     *
     * @param std::vector<int> formation The platoon formation.
     */
    UpdatePlatoonFormation* createUpdatePlatoonFormation(const std::vector<int>& formation);
};

} // namespace plexe

#endif
