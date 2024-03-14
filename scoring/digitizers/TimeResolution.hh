// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "DigitizerModule.hh"
#include "Pulse.hh"

// https://opengate.readthedocs.io/en/latest/digitizer_and_detector_modeling.html#time-resolution
class TimeResolution : public DigitizerModule {
public:
  TimeResolution(G4double resolution);
  ~TimeResolution() = default;
  void ProcessPulses(std::vector<Pulse>, std::vector<Pulse> &) override;

protected:
  void ProcessPulse(Pulse, std::vector<Pulse> &) override;

private:
  static G4double fwhmToStddevFactor;
  G4double stddev;
};
