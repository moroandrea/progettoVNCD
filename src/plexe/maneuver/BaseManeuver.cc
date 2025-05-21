#include "plexe/maneuver/BaseManeuver.h"
#include "plexe/apps/BasePlatooningApp.h"

namespace plexe {

BaseManeuver::BaseManeuver(BasePlatooningApp* app)
    : app(app)
    , positionHelper(app->getPositionHelper())
    , mobility(app->getMobility())
    , traci(app->getTraci())
    , traciVehicle(app->getTraciVehicle())
    , plexeTraci(app->getPlexeTraci())
    , plexeTraciVehicle(app->getPlexeTraciVehicle())
{
}

} // namespace plexe
