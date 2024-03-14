// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "SigmoidalThresholder.hh"

#include "Randomize.hh"
#include "globals.hh"
#include <math.h>

SigmoidalThresholder::SigmoidalThresholder(G4double threshold, G4double alpha,
                                         G4double percent)
    : threshold(threshold), alpha(alpha), percent(percent) {}

void SigmoidalThresholder::ProcessPulse(Pulse pulse,
                                       std::vector<Pulse> &pulses) {

  G4double sigma =
      1.0 / (1 + exp(alpha * (pulse.energy - threshold) / threshold));

  G4double random = G4UniformRand();

  if (sigma > random) {
    pulses.push_back(pulse);
  }

  // otherwise, discard
}
