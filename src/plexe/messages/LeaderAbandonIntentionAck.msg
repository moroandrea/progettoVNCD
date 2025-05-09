import ManeuverMessage;

// ACK for the LeaderAbandonIntention message.
// Sent by followers to confirm that they received the leader's intention to leave.
packet LeaderAbandonIntentionAck extends ManeuverMessage {
    double platoonSpeed;
    int platoonLane;
    int newPlatoonFormation[];
}
