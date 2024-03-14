// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once

#include "Coincidence.hh"
#include "CoincidenceCounter.hh"
#include "DigitizerModule.hh"
#include "DigitizerScorerCrystalOnly.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"
#include "Pulse.hh"
#include "TsVNtupleScorer.hh"

class PETScorerTest : public DigitizerScorerCrystalOnly
{
public:
  PETScorerTest(TsParameterManager *pM, TsMaterialManager *mM,
                TsGeometryManager *gM, TsScoringManager *scM,
                TsExtensionManager *eM, G4String scorerName, G4String quantity,
                G4String outFileName, G4bool isSubScorer);

  virtual ~PETScorerTest() override;

  void InitializeOutputColumns() override;
  void FillOutputColumns(std::vector<Pulse> *pulsesSoFar) override;

  void Output() override;
  void UserHookForEndOfRun() override;

protected:
  // Output variables
  G4double fXPos1;
  G4double fYPos1;
  G4double fZPos1;
  G4double fXPos2;
  G4double fYPos2;
  G4double fZPos2;
  G4double fEnergy1;
  G4double fEnergy2;
  G4String fName1;
  G4String fName2;
  G4int fCrystalIndex1;
  G4int fCrystalIndex2;
  G4double fTime;
  G4double fTimeDelta;
  G4double fRandomIntensityRate;

  // G4String fOrigin1;
  // G4String fOrigin2;
  // G4int fNbRuns;
  // G4int runCount;
  std::ofstream foutputFile;
  G4double fminTime;
  G4double fmaxTime;
  G4double fminTimeDelta;
  G4double fmaxTimeDelta;
  G4int fCoincidenceCounter;
  // PET parameters
  G4double fTimeResolution;
  G4double fDeadTime;
  G4double fMinEnergy;
  G4double fMaxEnergy;
  G4double fCoincidenceTimeWindowPrompts;
  G4double fCoincidenceTimeWindowRandoms;
  G4double fCoincidenceTimeDelayRandoms;
  G4double fMinDistanceInCoincidence;

  G4double fXOrigin1;
  G4double fYOrigin1;
  G4double fZOrigin1;
  G4double fXOrigin2;
  G4double fYOrigin2;
  G4double fZOrigin2;
  // G4double fCreationTime1;
  // G4double fCreationTime2;
  G4double fTime1;
  G4double fTime2;
  G4int fEventID1;
  G4int fEventID2;

  G4int fSubmoduleIndex1;
  G4int fSubmoduleIndex2;
  G4int fModuleIndex1;
  G4int fModuleIndex2;
  G4int fDetectorIndex1;
  G4int fDetectorIndex2;

  // Data for tracking when to consolidate data
  G4int fRunConsolidationFrequency;
  G4bool fConsolidateRuns;

  std::vector<Coincidence> fCoincidencesPrompts;
  std::vector<Coincidence> fCoincidencesRandoms;

  // CASToR parameters
  G4double fCastorTOFResolution;
  G4bool fCastorIncludeTOF;
  G4bool fCastorIncludeRandomsCorrection;

  // CASToR output variables for list-mode event
  uint32_t fTimeCastor; // ms
  float fAttenuationCorrectionFactorCastor;
  float fUnnormalizedScatterIntensityCastor; // count/s
  float fUnnormalizedRandomIntensityCastor;  // count/s
  float fNormalizationFactorCastor;
  float fTimeDeltaCastor; // ps
  // uint16_t fNbCrystalPairsCastor; //not used, but kept here in case it's ever
  // needed. Note: TOPAS does not support 16 bit integers in binary output at
  // the moment so you may have to split into two 8 bit ints if you want this
  uint32_t fCrystalID1Castor;
  uint32_t fCrystalID2Castor;

protected:
  CoincidenceCounter *coincidenceCounterPrompts;
  CoincidenceCounter *coincidenceCounterRandoms;
  void CoincidenceDetection(std::vector<Pulse> *singles,
                            std::vector<Coincidence> &coincidencesPrompts,
                            std::vector<Coincidence> &coincidencesRandoms,
                            G4bool onlyDoStart, G4bool garbageCollect);
};
