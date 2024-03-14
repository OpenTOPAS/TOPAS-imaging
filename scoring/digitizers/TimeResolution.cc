// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "TimeResolution.hh"

#include "Randomize.hh"
#include "globals.hh"
#include <algorithm>
#include <cmath>

// https://en.wikipedia.org/wiki/Full_width_at_half_maximum
G4double TimeResolution::fwhmToStddevFactor = 1 / (2 * sqrt(2 * log(2)));

// resolution is FWHM and mean is input energy
TimeResolution::TimeResolution(G4double resolution)
    : stddev(resolution * fwhmToStddevFactor) {}

void TimeResolution::ProcessPulses(std::vector<Pulse> pulses,
                                   std::vector<Pulse> &newPulses) {
  for (auto pulse : pulses) {
    G4double timeDelta = G4RandGauss::shoot(0, stddev);
    // G4cout << "time delta: " << timeDelta << G4endl;
    newPulses.push_back(Pulse(pulse, timeDelta, 0, G4ThreeVector(0, 0, 0)));
  }
  
  std::sort(newPulses.begin(), newPulses.end(),
            [&](Pulse pulse1, Pulse pulse2) {
              return pulse1.endTimeTopas < pulse2.endTimeTopas;
            });
}

void TimeResolution::ProcessPulse(Pulse pulse, std::vector<Pulse> &pulses) {
  G4double timeDelta = G4RandGauss::shoot(0, stddev);

  // G4cout << "time delta: " << timeDelta << G4endl;

  Pulse newPulse = Pulse(pulse, timeDelta, 0, G4ThreeVector(0, 0, 0));

  auto pulseIterator = upper_bound(
      pulses.begin(), pulses.end(), newPulse, [&](Pulse pulse1, Pulse pulse2) {
        return pulse1.endTimeTopas < pulse2.endTimeTopas;
      });
  pulses.insert(pulseIterator, newPulse);
}
