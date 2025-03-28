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

namespace plexe {

Define_Module(SimplePlatooningApp);

void SimplePlatooningApp::initialize(int stage)
{

}

void SimplePlatooningApp::handleLowerMsg(cMessage* msg)
{

}

void SimplePlatooningApp::sendUnicast(cPacket* msg, int destination)
{

}

void SimplePlatooningApp::sendLeaderAbandonIntention()
{
    getSimulation()->getActiveEnvir()->alert("DAJE che siamo a meta progetto");
    // mandare un messaggio a tutti
}

void SimplePlatooningApp::sendLeaderIntentionToLeader()
{

}

void SimplePlatooningApp::broadcastFormationUpdate(std::vector<int>& formation)
{

}

void SimplePlatooningApp::handleLeaderAbandonIntention(const LeaderAbandonIntention* msg)
{

}

void SimplePlatooningApp::handleReadyToBecomeLeader(const ReadyToBecomeLeader* msg)
{

}

void SimplePlatooningApp::handleUpdateFormation(const UpdateFormation* msg)
{

}

LeaderAbandonIntention* SimplePlatooningApp::createLeaderAbandonIntentionMsg()
{

}

ReadyToBecomeLeader* SimplePlatooningApp::createReadyToBecomeLeaderMsg()
{

}

UpdateFormation* SimplePlatooningApp::createUpdateFormationMsg(const std::vector<int>& formation)
{

}

void SimplePlatooningApp::setPlatoonRole(PlatoonRole r)
{
    role = r;
}

} // namespace plexe
