//
// Copyright (C) 2018-2023 Julian Heinovski <julian.heinovski@ccs-labs.org>
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

#include "plexe/scenarios/SimpleScenario.h"
#include "plexe/utilities/BasePositionHelper.h"

using namespace veins;

namespace plexe {

Define_Module(SimpleScenario);

void SimpleScenario::initialize(int stage)
{
    BaseScenario::initialize(stage);

    if (stage == 0) {
        // find application
        app = FindModule<SimplePlatooningApp*>::findSubModule(getParentModule());
    }

    if (stage == 2) {
        leaderSpeed = par("leaderSpeed").doubleValue() / 3.6;
        exitSpeed = par("exitSpeed").doubleValue() / 3.6;
        exitLane = par("exitLaneIndex").intValue();

        // remember starting role
        lastRole = PlatoonRole::NONE;

        if (positionHelper->isLeader()) {
            lastRole = PlatoonRole::LEADER;

            // setup start maneuver trigger and exit
            startManeuver = new cMessage("startManeuver");
            exitEvent = new cMessage("exitEvent");

            // leader sets cruise speed and schedules start
            plexeTraciVehicle->setCruiseControlDesiredSpeed(leaderSpeed);
            double when = par("whenToStartManeuver").doubleValue();
            scheduleAt(when, startManeuver);

            // setup role watcher
            watchRoleMsg = new cMessage("watchRole");
            scheduleWatch(0.1);
        }
        else {
            // followers speed up to maintain gap
            plexeTraciVehicle->setCruiseControlDesiredSpeed(leaderSpeed + 10);
        }
    }
}

void SimpleScenario::handleMessage(cMessage* msg)
{
    if (msg == startManeuver) {
        app->startLeaderReplacementManeuver();
    }
    else if (msg == watchRoleMsg) {
        checkRole();
        scheduleWatch(0.1);
    }
    else if (msg == exitEvent) {
        std::ostringstream alertMsg;
        alertMsg << "[Vehicle:" << positionHelper->getId() << " | Platoon: " << positionHelper->getPlatoonId()
                 << "] Exited platoon, now accelerating and changing lane.";
        getSimulation()->getActiveEnvir()->alert(alertMsg.str().c_str());

        plexeTraciVehicle->setCruiseControlDesiredSpeed(exitSpeed);
        plexeTraciVehicle->changeLane(exitLane, 3.0);
    }
    else {
        BaseScenario::handleSelfMsg(msg);
    }
}

void SimpleScenario::scheduleWatch(double delay)
{
    scheduleAt(simTime() + delay, watchRoleMsg);
}

void SimpleScenario::checkRole()
{
    PlatoonRole current = app->getPlatoonRole();
    if (lastRole == PlatoonRole::LEADER && current == PlatoonRole::NONE) {
        scheduleAt(simTime() + 1.0, exitEvent);
    }
    lastRole = current;
}

} // namespace plexe
