#ifndef BASEPLATOONAPP_H_
#define BASEPLATOONAPP_H_

#include "plexe/apps/BaseApp.h"

#include "plexe/maneuver/BaseManeuver.h"
#include "plexe/messages/ManeuverMessage_m.h"
#include "plexe/scenarios/BaseScenario.h"

namespace plexe {

/** possible roles of this vehicle */
enum class PlatoonRole : size_t {
    NONE, ///< The vehicle is not in a Platoon
    LEADER, ///< The vehicle is the leader of its Platoon
    FOLLOWER, ///< The vehicle is a normal follower in its Platoon
    JOINER ///< The vehicle is in the process of joining a Platoon
};

/**
 * Base application for Platoons.
 */
class BasePlatooningApp : public BaseApp {

public:
    BasePlatooningApp()
    : inManeuver(false)
    , activeManeuver(nullptr)
    , scenario(nullptr)
    , role(PlatoonRole::NONE)
    {
    }

    /** override from BaseApp */
    virtual void initialize(int stage) override;

    /**
     * Returns the role of this car in the platoon
     *
     * @return PlatoonRole the role in the platoon
     * @see PlatoonRole
     */
    const PlatoonRole& getPlatoonRole() const
    {
        return role;
    }

    /**
     * Sets the role of this car in the platoon
     *
     * @param PlatoonRole r the role in the platoon
     * @see PlatoonRole
     */
    void setPlatoonRole(PlatoonRole r);

    /**
     * Sends a unicast message
     *
     * @param cPacket msg message to be encapsulated into the unicast
     * message
     * @param int destination of the message
     */
    virtual void sendUnicast(cPacket* msg, int destination);

    BasePositionHelper* getPositionHelper()
    {
        return positionHelper;
    }

    veins::TraCIMobility* getMobility()
    {
        return mobility;
    }

    veins::TraCICommandInterface* getTraci()
    {
        return traci;
    }

    veins::TraCICommandInterface::Vehicle* getTraciVehicle()
    {
        return traciVehicle;
    }

    traci::CommandInterface* getPlexeTraci()
    {
        return plexeTraci;
    }

    traci::CommandInterface::Vehicle* getPlexeTraciVehicle()
    {
        return plexeTraciVehicle.get();
    }

protected:
    virtual void handleLowerMsg(cMessage* msg) override;

    /**
     * Handles maneuver messages.
     *
     * @param mm maneuver message to handle
     */
    virtual void onManeuverMessage(ManeuverMessage* mm);

    /** am i in a maneuver? */
    bool inManeuver;

    /** which maneuver is currently active? */
    BaseManeuver* activeManeuver;

    BaseScenario* scenario;

private:
    /** the role of this vehicle */
    PlatoonRole role;
};

} // namespace plexe

#endif
