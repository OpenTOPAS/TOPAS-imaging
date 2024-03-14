// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "EnergyCutoffModule.hh"

#include "globals.hh"

EnergyCutoffModule::EnergyCutoffModule(G4double minEnergyWindow, G4double maxEnergyWindow) :
    minEnergy(minEnergyWindow), maxEnergy(maxEnergyWindow) {}

void EnergyCutoffModule::ProcessPulse(Pulse pulse, std::vector<Pulse>& pulses) {
    // G4cout << "min energy: " << minEnergy << " current energy " << pulse.energy << " max energy: " << maxEnergy << G4endl;
    if (pulse.energy < maxEnergy && pulse.energy > minEnergy) {
        pulses.push_back(pulse);
    }
}
