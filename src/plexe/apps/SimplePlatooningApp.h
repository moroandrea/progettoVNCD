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

#ifndef SIMPLEPLATOONINGAPP_H_
#define SIMPLEPLATOONINGAPP_H_

#include "plexe/apps/BasePlatooningApp.h"

#include "plexe/maneuver/LeaderReplacement.h"

namespace plexe {

class SimplePlatooningApp : public BasePlatooningApp {

public:
    SimplePlatooningApp()
    : BasePlatooningApp()
    {
    }

    virtual ~SimplePlatooningApp();

    virtual void startLeaderReplacementManeuver();

protected:
    virtual void initialize(int stage) override;

    virtual void handleLowerMsg(cMessage* msg) override;

    /**
     * Handles maneuver messages.
     *
     * @param mm maneuver message to handle
     */
    virtual void onManeuverMessage(ManeuverMessage* mm) override;

private:
    LeaderReplacement* leaderReplacementManeuver;
};

} // namespace plexe

#endif /* SIMPLEPLATOONINGAPP_H_ */
