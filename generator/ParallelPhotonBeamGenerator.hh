// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "TsVGenerator.hh"

class ParallelPhotonBeamGenerator : public TsVGenerator {
public:
  ParallelPhotonBeamGenerator(TsParameterManager *pM, TsGeometryManager *gM,
                              TsGeneratorManager *pgM, G4String sourceName);
  ~ParallelPhotonBeamGenerator() = default;

  void ResolveParameters();
  void GeneratePrimaries(G4Event *);

private:
  G4double fXpos;
  G4double fYpos;
  G4double fZpos;

  G4double fAngleError;
  G4double fMeanPositronRange;
};
