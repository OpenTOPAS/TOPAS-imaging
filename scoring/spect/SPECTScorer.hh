// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "DigitizerScorerCrystalOnly.hh"

class SPECTScorer : public DigitizerScorerCrystalOnly {
public:
  SPECTScorer(TsParameterManager *pM, TsMaterialManager *mM,
              TsGeometryManager *gM, TsScoringManager *scM,
              TsExtensionManager *eM, G4String scorerName, G4String quantity,
              G4String outFileName, G4bool isSubScorer);

  virtual ~SPECTScorer();

  void InitializeOutputColumns() override;
  void FillOutputColumns(std::vector<Pulse>* pulsesSoFar) override;

protected:
  // Output variables
  G4double fXPos;
  G4double fYPos;
  G4double fZPos;
  G4double fEnergy;
  G4int fCopyNr;

  G4double fTimeResolution;
  G4double fMinEnergy;
  G4double fMaxEnergy;

  uint32_t fTimeCastor;
  float fDataInBin;
  // float fCastorScatterIntensity;
  // float fCastorNormalization;
  uint32_t fProjectionIdCastor;
  uint32_t fBinIdCastor;

  G4int fCastorTransaxialBins;
  G4int fCastorAxialBins;

  G4int NbOfDetectors;
};
