// Extra Class for use by PETScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "CoincidenceCounter.hh"

#include "Coincidence.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

CoincidenceCounterDistance::CoincidenceCounterDistance(G4double minDistance)
    : fMinDistance(minDistance) {}

void CoincidenceCounterDistance::AddCoincidences(
    std::vector<Coincidence> &coincidences, std::vector<Pulse> pulses) {

  const size_t N = pulses.size();
  for (int i = 0; i < N; i++) {
    for (int j = i + 1; j < N; j++) {
      G4ThreeVector pos1 = pulses[i].position;
      G4ThreeVector pos2 = pulses[j].position;

      G4double dist = (pos1 - pos2).mag();
      // G4cout<<"CoincidenceCounter: Dist "<<dist<<G4endl;
      // exclude pulses that are too far away
      if (dist < fMinDistance) {
      	// G4cout << "Skipping!!" << fMinDistance << G4endl;
        continue;
      }

      /* filtering to only use trues
      if(pulses[i].eventID != pulses[j].eventID) {
      	G4cout << "skipping not same event" << G4endl;
      	continue;
      }
      */

      // TODO: implement more rule options
      // right now, it takes all pairs, the default of GATE
      coincidences.push_back(Coincidence(pulses[i], pulses[j]));
    }
  }
}
