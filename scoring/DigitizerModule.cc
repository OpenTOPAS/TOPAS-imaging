// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "DigitizerModule.hh"

#include "globals.hh"

void DigitizerModule::ProcessPulses(std::vector<Pulse> pulsesSoFar,
                                    std::vector<Pulse> &newPulses) {
  G4cout<<"ProcessPulses"<<G4endl;	
  for (auto pulse : pulsesSoFar) {
  	if(!pulse.beenDigitized) {
  		ProcessPulse(pulse, newPulses);
  	} else {
  		G4cout << "skipped pulse to digitize" << G4endl;
  	}
  }
}
