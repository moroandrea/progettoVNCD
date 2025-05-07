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

#include "plexe/apps/BaseApp.h"

#include "plexe/messages/LeaderAbandonIntention_m.h"
#include "plexe/messages/ReadyToBecomeLeader_m.h"
#include "plexe/messages/UpdatePlatoonFormation_m.h"

#include "plexe/scenarios/BaseScenario.h"

namespace plexe {

enum class PlatoonRole : size_t {
    NONE, ///< The vehicle is not in a Platoon
    LEADER, ///< The vehicle is the leader of its Platoon
    FOLLOWER, ///< The vehicle is a normal follower in its Platoon
    JOINER ///< The vehicle is in the process of joining a Platoon
};

class SimplePlatooningApp : public BaseApp {

public:
    SimplePlatooningApp()
    : scenario(nullptr)
    , role(PlatoonRole::NONE)
    {
    }

    /**
     * Sends the intention to abandon the platoon.
     */
    void sendLeaderAbandonIntention();

    virtual void sendUnicast(cPacket* msg, int destination);

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

protected:
    virtual void initialize(int stage) override;
    virtual void handleLowerMsg(cMessage* msg) override;

    BaseScenario* scenario;

private:
    /**
     * Creates a LeadearAbandonIntention message.
     */
    LeaderAbandonIntention* createLeaderAbandonIntentionMsg();

    /**
     * Creates a ReadyToBecomeLeader message.
     */
    ReadyToBecomeLeader* createReadyToBecomeLeaderMsg();

    /**
     * Creates a UpdatePlatoonFormation message, using the specified platoon formation.
     *
     * @param std::vector<int> formation The platoon formation.
     */
    UpdatePlatoonFormation* createUpdateFormationMsg(const std::vector<int>& formation);

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
    void handleUpdateFormation(const UpdatePlatoonFormation* msg);

    /**
     * Fills the ManeuverMessage with all the basic information.
     *
     * @param msg ManeuverMessage The message to be filled
     * @param int vehicleId The id of the sending vehicle
     * @param int platoonId The id of the platoon of the sending vehicle
     * @param int destinationId The id of the destination
     */
    void fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId);

    /**
     * Sends a platoon leader election proposal to the current
     * platoon leader.
     */
    void sendLeaderIntentionToLeader();

    /**
     * Sends the updated platoon formation to all other platoon
     * members.
     *
     * @param std::vector<int> formation The platoon formation.
     */
    void broadcastFormationUpdate(std::vector<int>& formation);

    /** the role of this vehicle */
    PlatoonRole role;
};

} // namespace plexe

#endif /* SIMPLEPLATOONINGAPP_H_ */
