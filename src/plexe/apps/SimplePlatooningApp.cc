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

#include "plexe/apps/SimplePlatooningApp.h"
#include "plexe/messages/PlexeInterfaceControlInfo_m.h"
#include "plexe/protocols/BaseProtocol.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/utility/Consts80211p.h"

using namespace veins;

namespace plexe {

Define_Module(SimplePlatooningApp);

void SimplePlatooningApp::initialize(int stage)
{
    BasePlatooningApp::initialize(stage);

    if (stage == 1) {
        leaderReplacementManeuver = new LeaderReplacement(this);
    }
}

void SimplePlatooningApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(msg);

    cPacket* enc = frame->getEncapsulatedPacket();
    ASSERT2(enc, "received a BaseFrame1609_4s with nothing inside");

    if (enc->getKind() == MANEUVER_TYPE) {
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(frame->decapsulate());

        onManeuverMessage(mm);

        delete frame;
    }
    else {
        BasePlatooningApp::handleLowerMsg(msg);
    }
}

void SimplePlatooningApp::startLeaderReplacementManeuver()
{
    ASSERT(getPlatoonRole() == PlatoonRole::LEADER);

    leaderReplacementManeuver->startManeuver();
}

void SimplePlatooningApp::onManeuverMessage(ManeuverMessage* mm)
{
    if (activeManeuver) {
        activeManeuver->onManeuverMessage(mm);
    }
    else {
        leaderReplacementManeuver->onManeuverMessage(mm);
    }
    delete mm;
}

SimplePlatooningApp::~SimplePlatooningApp()
{
    delete leaderReplacementManeuver;
}

} // namespace plexe
