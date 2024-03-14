// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "PileUpModule.hh"

#include "globals.hh"

PileUpModule::PileUpModule(G4double pileUpTimeWindow) 
    : pileUpTimeWindow(pileUpTimeWindow),
      // set to time far back enough to not interfere with the first pulse
      currentPileUpTimestamp(-2 * pileUpTimeWindow) {}

void PileUpModule::ProcessPulse(Pulse pulse, std::vector<Pulse> &pulses) {
  G4double timestamp = pulse.endTimeTopas;

  // inside time window
  if (timestamp - currentPileUpTimestamp <= pileUpTimeWindow) {
    // empty array met criteria - this shouldn't happen
    if (pulses.empty()) {
      G4cerr << "Empty pulse list in PileUpModule where pulse in piling up "
                "time window. Are there negative timestamps?" << G4endl;
      G4cerr << "Time window start time: " << currentPileUpTimestamp << G4endl;
      G4cerr << "Input pulse timestamp: " << timestamp << G4endl;
      G4cerr << "Pile up time window: " << pileUpTimeWindow << G4endl;
    } else {
      Pulse lastPulse = pulses.back();
      pulses.pop_back();

      Pulse fusedPulse = Pulse(lastPulse, pulse);

      pulses.push_back(fusedPulse);

    }
  } else { // outside time window
    pulses.push_back(pulse);
    currentPileUpTimestamp = timestamp;
  }
}
