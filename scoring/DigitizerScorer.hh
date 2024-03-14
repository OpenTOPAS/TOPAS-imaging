// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "DigitizerModule.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"
#include "Pulse.hh"
#include "TsVNtupleScorer.hh"
#include <utility>
#include <vector>
#include <map>

#define ID std::pair<G4String, G4int>

class DigitizerScorer : public TsVNtupleScorer {
public:
  DigitizerScorer(G4bool isInherited, TsParameterManager *pM,
                  TsMaterialManager *mM, TsGeometryManager *gM,
                  TsScoringManager *scM, TsExtensionManager *eM,
                  G4String scorerName, G4String quantity, G4String outFileName,
                  G4bool isSubScorer);

  DigitizerScorer(TsParameterManager *pM, TsMaterialManager *mM,
                  TsGeometryManager *gM, TsScoringManager *scM,
                  TsExtensionManager *eM, G4String scorerName,
                  G4String quantity, G4String outFileName, G4bool isSubScorer);

  ~DigitizerScorer();

  G4bool ProcessHits(G4Step *, G4TouchableHistory *);

  void UserHookForEndOfEvent();
  virtual void UserHookForEndOfRun();

  virtual void InitializeOutputColumns();
  virtual void FillOutputColumns(std::vector<Pulse>* pulsesSoFar);

  void InitializeRotationAngleTracking();
  void RegisterRotationAngles();
  void PositionCorrection();

protected:

  virtual void ApplyDigitizers();

  // Output variables
  G4double fEnergy;
  G4int fEventID;

  G4double fStartGlobalTime;
  G4double fEndGlobalTime;
  G4double fTopasTime;
  G4double fXPos;
  G4double fYPos;
  G4double fZPos;

  G4int fNbRuns;
  G4int runCount;

  // CASToR parameters
  G4bool fIncludeCastorOutput;
  G4String fCastorScannerName;
  G4String fCastorOutputFilename;
  G4double fCastorCalibrationFactor;
  G4String fCastorIsotope;

  // To be filled in by SPECT/CT/PG or ignored
  G4int fProjectionID;
  std::vector<G4double> fRotationAngles;

  // Keeping track of the pulses
  std::vector<std::pair<ID, Pulse>> pulses;
  static std::map<G4String, std::vector<Pulse>> addedPulsesTotal;
  std::vector<Pulse>* addedPulses;

  G4String fScorerName;

  //artificial offsets
  static std::map<G4int, G4double> eventOffsets;
  G4double fOffsetWindow;

  // Digitizer modules
  static std::map<G4String, std::vector<DigitizerModule *>> digitizersTotal;
  std::vector<DigitizerModule *>* digitizers;

  // Making sure that all the static variables are accessed at most once by the same thread
  static G4Mutex aMutex;
};
