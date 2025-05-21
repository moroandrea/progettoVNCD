#ifndef BASEMANEUVER_H_
#define BASEMANEUVER_H_

#include "plexe/messages/ManeuverMessage_m.h"
#include "plexe/mobility/CommandInterface.h"
#include "plexe/utilities/BasePositionHelper.h"

#include "veins/modules/mobility/traci/TraCIMobility.h"

namespace plexe {

class BasePlatooningApp;

class BaseManeuver {

public:
    /**
     * Constructor
     *
     * @param app pointer to the base platooning application used to fetch parameters and inform it about a concluded maneuver
     */
    BaseManeuver(BasePlatooningApp* app);
    virtual ~BaseManeuver(){};

    /**
     * This method is invoked by the base platooning application to start the maneuver
     *
     * @param parameters parameters passed to the maneuver
     */
    virtual void startManeuver(const void* parameters) = 0;

    /**
     * This method is invoked by the base platooning application to abort the maneuver
     */
    virtual void abortManeuver() = 0;

    /**
     * This method is invoked by the base platooning application when a maneuver message is received.
     * The maneuver must not free the memory of the message, as this might be needed by other maneuvers as well.
     */
    virtual void onManeuverMessage(const ManeuverMessage* mm) = 0;

protected:
    BasePlatooningApp* app;
    BasePositionHelper* positionHelper;
    veins::TraCIMobility* mobility;
    veins::TraCICommandInterface* traci;
    veins::TraCICommandInterface::Vehicle* traciVehicle;
    traci::CommandInterface* plexeTraci;
    traci::CommandInterface::Vehicle* plexeTraciVehicle;
};

} // namespace plexe

#endif
