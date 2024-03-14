//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.
#pragma once

#include "DigitizerScorer.hh"

class DigitizerScorerCrystalOnly : public DigitizerScorer
{
public:
  DigitizerScorerCrystalOnly(TsParameterManager *pM, TsMaterialManager *mM,
                             TsGeometryManager *gM, TsScoringManager *scM,
                             TsExtensionManager *eM, G4String scorerName,
                             G4String quantity, G4String outFileName,
                             G4bool isSubScorer);
  DigitizerScorerCrystalOnly(TsParameterManager *pM, TsMaterialManager *mM,
                             TsGeometryManager *gM, TsScoringManager *scM,
                             TsExtensionManager *eM, G4String scorerName,
                             G4String quantity, G4String outFileName,
                             G4bool isSubScorer, G4String CrystalBoxLVName);
  virtual ~DigitizerScorerCrystalOnly() = default;
  bool ProcessHits(G4Step *, G4TouchableHistory *);

protected:
  G4String fDetectorName;
  G4String fCrystalLVName;

  G4LogicalVolume *fCrystalBoxLV = nullptr;
};
