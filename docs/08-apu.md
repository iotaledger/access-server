# Policy Update Protocol
The Figure below shows a visual representation of the Policy Update Protocol:

ToDo: refactor this.

![drawing](/docs/images/update.png)
1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. PAP sends Storage Checksum to Network Actor, who forwards it to the Tangle Policy Store (TPS). If ID sent by PAP matches with the Checksum of Policy List stored on the TPS, then TPS replies ok (nothing to update). If the ID differs, the TPS replies with Policy List (update required).
4. PAP Updater compares the received Policy List with all Policies stored locally. This is done via PAP Plugin.
5. PAP initiates the request for the Policies that differ. Request is relayed to Network Actor, who forwards it to TPS. TPS fetches Policy from IOTA Permanode and replies back.
6. PAP parses and validates the new Policies it receives.
7. PAP stores new Policies via PAP Plugin. PAP goes back to Checksum poll mode.

Note: The protocol described in this section will eventually evolve into a different approach. The entity called `Tangle Policy Store` is a Cloud Server that was inherited by the Legacy Design of early FROST implementations. Eventually this centralized entity will be completely removed and Policy Updates will be done via interactions between Access Actor and Wallet Actor, where the Wallet communicates with an IOTA Permanode.

#### Policy Validation
Policy Validation is the process where Policy structure is checked in order to:
  - Validate JSON.
  - Eliminate reduncancy of a binary circuit.
  - Limit values checks.
  - Check for suspicious conditions.
