// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once

#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "TsTrackInformation.hh"
#include "globals.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <G4VProcess.hh>
#include "G4ParticleTable.hh"

struct Pulse
{
  // fuses two pulses
  Pulse(Pulse, Pulse);
  // shifts in the time domain, energy, and position
  Pulse(Pulse, G4double timeShift, G4double energyShift,
        G4ThreeVector positionShift);
  Pulse(G4Step *aStep, G4double eventStartTimeTopas, G4int projectionId = 0, G4int _eventID = 0);
  Pulse() = default;
  ~Pulse() = default;

  void Add(G4Step *);

  // in future, maybe average positions of particles.
  // It's unclear to me if i should take the crystal position or some position
  // inside the crystal
  G4ThreeVector position;
  G4double energy;
  G4double startTimeGlobal;
  G4double endTimeGlobal;

  G4double eventStartTimeTopas;
  G4double startTimeTopas;
  G4double endTimeTopas;
  G4String physicalVolumeName;
  
  G4int projectionId; // for SPECT, CBCT, and PG
  G4int eventID;

  G4ThreeVector OriginPosition;
  G4int crystalIndex;
  G4int submoduleIndex;
  G4int moduleIndex;
  G4int detectorIndex;
  G4int historyDepth;
  // G4double creation_time;

  // whether or not this pulse has been put through the digitizers yet
  G4bool beenDigitized;
};
