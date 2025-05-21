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

#ifndef SIMPLESCENARIO_H_
#define SIMPLESCENARIO_H_

#include "plexe/scenarios/BaseScenario.h"
#include "plexe/apps/SimplePlatooningApp.h"

namespace plexe {

class SimpleScenario : public BaseScenario {
public:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;

    SimpleScenario()
        : leaderSpeed(0)
        , app(nullptr)
        , startManeuver(nullptr)
        , watchRoleMsg(nullptr)
        , lastRole(PlatoonRole::NONE)
        , exitSpeed(0)
        , exitLane(1)
    {
    }

protected:
    // average leader cruising speed (m/s)
    double leaderSpeed;

    // pointer to application layer
    SimplePlatooningApp* app;

private:
    /** message to trigger the maneuver start */
    cMessage* startManeuver;

    /** periodic watcher to detect role change */
    cMessage* watchRoleMsg;

    /** message to trigger previous leader speedup */
    cMessage* exitEvent;

    /** previous platoon role */
    PlatoonRole lastRole;

    /** speed to adopt when exiting platoon */
    double exitSpeed;

    /** relative lane offset on exit (+1 = one lane right) */
    int exitLane;

    /** schedule next watch invocation */
    void scheduleWatch(double delay);
    /** check for LEADER->NONE transition */
    void checkRole();
};

} // namespace plexe

#endif
