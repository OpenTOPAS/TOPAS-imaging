// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "Coincidence.hh"
#include "Pulse.hh"
#include "globals.hh"

class CoincidenceCounter {
public:
  CoincidenceCounter() = default;
  ~CoincidenceCounter() = default;

  virtual void AddCoincidences(std::vector<Coincidence> &,
                               std::vector<Pulse>) = 0;
};

class CoincidenceCounterDistance : public CoincidenceCounter {
public:
  CoincidenceCounterDistance(G4double minDistance);
  void AddCoincidences(std::vector<Coincidence> &, std::vector<Pulse>) override;

protected:
  G4double fMinDistance;
};
