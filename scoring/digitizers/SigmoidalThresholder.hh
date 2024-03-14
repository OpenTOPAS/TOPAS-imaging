// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "DigitizerModule.hh"
#include "Pulse.hh"

// https://opengate.readthedocs.io/en/latest/digitizer_and_detector_modeling.html#sigmoidal-thresholder
class SigmoidalThresholder : public DigitizerModule {
public:
  SigmoidalThresholder(G4double threshold, G4double alpha, G4double percent);
  ~SigmoidalThresholder() = default;

protected:
  void ProcessPulse(Pulse, std::vector<Pulse> &) override;

private:
  G4double threshold;
  G4double alpha;
  G4double percent;
};
