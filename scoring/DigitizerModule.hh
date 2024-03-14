// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "Pulse.hh"
#include <vector>

// abstract class
/*
DigitizerModules work by being given all the pulses that have to be processed and putting the output into the given vector
The ProcessPulses is implemented by DigitizerModule and goes through each pulse to apply their custom ProcessPulse function.
This is well suited to processes that act on one pulse at a time, but it is possible to create digitizers that have more
memory that can have rules about multiple pulses.
Pulses are given in ascending time order
*/
class DigitizerModule {
public:
  DigitizerModule() = default;
  ~DigitizerModule() = default;

  virtual void ProcessPulses(std::vector<Pulse>, std::vector<Pulse> &);

protected:
  virtual void ProcessPulse(Pulse, std::vector<Pulse> &) = 0;
};
