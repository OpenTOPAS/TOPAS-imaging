// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.
#include "PETCrystalCounter.hh"

PETCrystalCounter::PETCrystalCounter(int NbOfDetectors, int NbOfTransaxialModules,
    int NbOfLayerModules, int NbOfAxialModules, int NbOfTransaxialSubmodules,
    int NbOfLayerSubmodules, int NbOfAxialSubmodules, int NbOfXBins, int NbOfYBins, int NbOfZBins) :
    NbOfDetectors(NbOfDetectors),
    NbOfAxialModules(NbOfAxialModules), NbOfLayerModules(NbOfLayerModules),
    NbOfTransaxialModules(NbOfTransaxialModules), NbOfAxialSubmodules(NbOfAxialSubmodules),
    NbOfLayerSubmodules(NbOfLayerSubmodules), NbOfTransaxialSubmodules(NbOfTransaxialSubmodules),
    NbOfAxialCrystals(NbOfZBins), NbOfLayerCrystals(NbOfYBins), NbOfTransaxialCrystals(NbOfXBins),
    NbOfCrystalsPerRing(
        NbOfDetectors * NbOfTransaxialModules * NbOfTransaxialSubmodules * NbOfTransaxialCrystals),
    NbOfCrystalsPerLayer(
        NbOfCrystalsPerRing * NbOfAxialCrystals * NbOfAxialSubmodules * NbOfAxialModules),
    NbOfCrystalsPerSubmodule(NbOfAxialCrystals * NbOfLayerCrystals * NbOfTransaxialCrystals),
    NbOfTransaxialCrystalPerAxialSubmodule(NbOfCrystalsPerRing * NbOfAxialCrystals),
    NbOfTransaxialCrystalPerLayerSubmodule(NbOfCrystalsPerLayer * NbOfLayerCrystals),
    NbOfTransaxialCrystalPerAxialModule(
        NbOfCrystalsPerRing * NbOfAxialCrystals * NbOfAxialSubmodules),
    NbOfTransaxialCrystalPerLayerModule(
        NbOfCrystalsPerLayer * NbOfLayerCrystals * NbOfLayerSubmodules),
    NbOfTransaxialCrystalPerRSector(
        NbOfTransaxialModules * NbOfTransaxialSubmodules * NbOfTransaxialCrystals),
    NbOfTransaxialCrystalPerModule(NbOfTransaxialSubmodules * NbOfTransaxialCrystals),
    NbOfModulesPerDetector(NbOfAxialModules * NbOfLayerModules * NbOfTransaxialModules),
    NbOfModulesPerRing(NbOfDetectors * NbOfTransaxialModules),
    NbOfModulesPerLayer(NbOfModulesPerRing * NbOfAxialModules),
    NbOfSubmodulesPerModule(NbOfAxialSubmodules * NbOfLayerSubmodules * NbOfTransaxialSubmodules),
    NbOfTransaxialSubmodulesPerRSector(NbOfTransaxialModules * NbOfTransaxialSubmodules),
    NbOfSubmodulesPerRing(NbOfDetectors * NbOfTransaxialModules * NbOfTransaxialSubmodules),
    NbOfSubmodulesPerLayer(NbOfSubmodulesPerRing * NbOfAxialSubmodules * NbOfAxialModules),
    NbOfSubmodulesPerAxialModule(NbOfSubmodulesPerRing * NbOfAxialSubmodules),
    NbOfSubmodulesPerLayerModule(NbOfSubmodulesPerLayer * NbOfLayerSubmodules) {}

PETCrystalCounter::PETCrystalCounter(int NbOfDetectors, int NbOfZBins, int NbOfXBins) :
    PETCrystalCounter(NbOfDetectors, 1, 1, 1, 1, 1, 1, NbOfXBins, 1, NbOfZBins) {}

PETCrystalCounter::PETCrystalCounter(int NbOfDetectors, int NbOfModuleX, int NbOfModuleY,
    int NbOfModuleZ, int NbOfZBins, int NbOfYBins, int NbOfXBins) :
    PETCrystalCounter(NbOfDetectors, NbOfModuleX, NbOfModuleY, NbOfModuleZ, 1, 1, 1, NbOfXBins,
        NbOfYBins, NbOfZBins) {}

void PETCrystalCounter::SetCurrentRing(int ringIndex) {
    fAxialRingSectorID = ringIndex;
}

void PETCrystalCounter::SetCurrentDetector(int detectorID) {
    fRingSectorID = detectorID;

    // only one module and one submodule
    fAxialModuleID         = 0;
    fTransaxialModuleID    = 0;
    fAxialSubmoduleID      = 0;
    fTransaxialSubmoduleID = 0;
    fBlockID               = fRingSectorID + fAxialRingSectorID * NbOfDetectors;
}

void PETCrystalCounter::SetDetectorID(int detectorID) {
    fRingSectorID = detectorID;
}

int PETCrystalCounter::GetModuleID(int moduleIDx, int moduleIDy, int moduleIDz) {
    SetModuleID(moduleIDx, moduleIDy, moduleIDz);
    // NbOfTransaxialModules = moduleIDx;
    // NbOfAxialModules = moduleIDz;
    // NbOfLayerModules = moduleIDy;
    // G4cout << "x: " << moduleIDx << " z: " << moduleIDz << " y: " << moduleIDy << G4endl;
    // G4cout << "Transaxial: " << fTransaxialModuleID << " Axial: " << fAxialModuleID << " Layer: " << fLayerModuleID << G4endl;
    int moduleID = fTransaxialModuleID;
    moduleID += fRingSectorID * NbOfTransaxialModules;
    moduleID += fAxialModuleID * NbOfModulesPerRing;
    moduleID += fLayerModuleID * NbOfModulesPerLayer;
    return moduleID;
}
void PETCrystalCounter::SetModuleID(int moduleIDx, int moduleIDy, int moduleIDz) {
    fTransaxialModuleID = moduleIDx;
    fLayerModuleID      = moduleIDy;
    fAxialModuleID      = moduleIDz;
}

int PETCrystalCounter::GetSubmoduleID(int submoduleIDx, int submoduleIDy, int submoduleIDz) {
    SetSubmoduleID(submoduleIDx, submoduleIDy, submoduleIDz);
    // G4cout << "x: " << submoduleIDx << " z: " << submoduleIDz << " y: " << submoduleIDy << G4endl;
    // G4cout << "Transaxial: " << fTransaxialSubmoduleID << " Axial: " << fAxialSubmoduleID << " Layer: " << fLayerSubmoduleID << G4endl;
    int submoduleID = fTransaxialSubmoduleID;
    submoduleID += fTransaxialModuleID * NbOfTransaxialSubmodules;
    submoduleID += fRingSectorID * NbOfTransaxialSubmodulesPerRSector;
    submoduleID += fAxialModuleID * NbOfSubmodulesPerAxialModule;
    submoduleID += fLayerModuleID * NbOfSubmodulesPerLayerModule;
    submoduleID += fAxialSubmoduleID * NbOfSubmodulesPerRing;
    submoduleID += fLayerSubmoduleID * NbOfSubmodulesPerLayer;
    return submoduleID;
}
void PETCrystalCounter::SetSubmoduleID(int submoduleIDx, int submoduleIDy, int submoduleIDz) {
    fTransaxialSubmoduleID = submoduleIDx;
    fLayerSubmoduleID      = submoduleIDy;
    fAxialSubmoduleID      = submoduleIDz;
}

int PETCrystalCounter::GetCrystalBoxID(int crystalIDx, int crystalIDy, int crystalIDz) {
    SetCrystalID(crystalIDx, crystalIDy, crystalIDz);
    // G4cout << "x: " << crystalIDx << " z: " << crystalIDz << " y: " << crystalIDy << G4endl;
    // G4cout << "Transaxial: " << fTransaxialModuleID << " Axial: " << fAxialModuleID << " Layer: " << fLayerModuleID << G4endl;
    // G4cout << "Transaxial: " << fTransaxialSubmoduleID << " Axial: " << fAxialSubmoduleID << " Layer: " << fLayerSubmoduleID << G4endl;
    // G4cout << "Transaxial: " << fTransaxialCrystalID << " Axial: " << fAxialCrystalID << " Layer: " << fLayerCrystalID << G4endl;
    int crystalID = fTransaxialCrystalID;
    crystalID += fTransaxialSubmoduleID * NbOfTransaxialCrystals;
    crystalID += fTransaxialModuleID * NbOfTransaxialCrystalPerModule;
    crystalID += fRingSectorID * NbOfTransaxialCrystalPerRSector;
    crystalID += fAxialSubmoduleID * NbOfTransaxialCrystalPerAxialSubmodule;
    crystalID += fLayerSubmoduleID * NbOfTransaxialCrystalPerLayerSubmodule;
    crystalID += fAxialModuleID * NbOfTransaxialCrystalPerAxialModule;
    crystalID += fLayerModuleID * NbOfTransaxialCrystalPerLayerModule;
    crystalID += fAxialCrystalID * NbOfCrystalsPerRing;
    crystalID += fLayerCrystalID * NbOfCrystalsPerLayer;
    return crystalID;
}

int PETCrystalCounter::GetCollimatorID(int collimatorIDx, int collimatorIDz) {
    int collimatorID = collimatorIDx;
    collimatorID += fTransaxialSubmoduleID * NbOfTransaxialCrystals;
    collimatorID += fTransaxialModuleID * NbOfTransaxialCrystalPerModule;
    collimatorID += fRingSectorID * NbOfTransaxialCrystalPerRSector;
    collimatorID += fAxialSubmoduleID * NbOfTransaxialCrystalPerAxialSubmodule;
    collimatorID += fAxialModuleID * NbOfTransaxialCrystalPerAxialModule;
    collimatorID += collimatorIDz * NbOfCrystalsPerRing;

    return collimatorID;
}
void PETCrystalCounter::SetCrystalID(int crystalIDx, int crystalIDy, int crystalIDz) {
    fTransaxialCrystalID = crystalIDx;
    fLayerCrystalID      = crystalIDy;
    fAxialCrystalID      = crystalIDz;
}

void PETCrystalCounter::SetCurrentDetector(int blockIDX, int blockIDY, int blockIDZ) {}
void PETCrystalCounter::GetCurrentModule(int blockIDX, int blockIDY, int blockIDZ) {
    fRingSectorID = blockIDZ;

    // only one module and one submodule
    fAxialModuleID         = blockIDX;
    fLayerModuleID         = blockIDY;
    fTransaxialModuleID    = blockIDZ;
    fAxialSubmoduleID      = 0;
    fLayerSubmoduleID      = 0;
    fTransaxialSubmoduleID = 0;
    fSubmoduleID           = 0;
    fBlockID               = fTransaxialModuleID;
    fBlockID += fRingSectorID * NbOfTransaxialModules;
    fBlockID += fLayerModuleID * NbOfModulesPerLayer;
}

int PETCrystalCounter::GetCrystalBoxID(int ZBin, int XBin) {
    // this is all based on CASToR standards for PET scanners
    fAxialCrystalID = ZBin;

    fRingID = fAxialCrystalID + fAxialSubmoduleID * NbOfAxialCrystals +
              fAxialModuleID * NbOfAxialSubmodules * NbOfAxialCrystals +
              fAxialRingSectorID * NbOfAxialModules * NbOfAxialSubmodules * NbOfAxialCrystals;

    fTransaxialCrystalID = XBin;

    return fRingID * NbOfCrystalsPerRing + fTransaxialCrystalID +
           fTransaxialSubmoduleID * NbOfTransaxialCrystals +
           fTransaxialModuleID * NbOfTransaxialSubmodules * NbOfTransaxialCrystals +
           fRingSectorID * NbOfTransaxialModules * NbOfTransaxialSubmodules *
               NbOfTransaxialCrystals;
}
// int PETCrystalCounter::GetCrystalBoxID(int ZBin, int YBin, int XBin)
// {
//   // this is all based on CASToR standards for PET scanners
//   fAxialCrystalID = ZBin;
//   fLayerCrystalID = YBin;
//   fTransaxialCrystalID = XBin;

//   fRingID = fAxialCrystalID;
//   fRingID += fAxialSubmoduleID * NbOfAxialCrystals;
//   fRingID += fAxialModuleID * NbOfAxialSubmodules * NbOfAxialCrystals;
//   fRingID += fAxialRingSectorID * NbOfAxialModules * NbOfAxialSubmodules * NbOfAxialCrystals;

//   fCrystalID = fRingID * NbOfCrystalsPerRing;
//   fCrystalID += fTransaxialCrystalID;
//   fCrystalID += fTransaxialSubmoduleID * NbOfTransaxialCrystals;
//   fCrystalID += fTransaxialModuleID * NbOfTransaxialSubmodules * NbOfTransaxialCrystals;
//   fCrystalID += fRingSectorID * NbOfTransaxialModules * NbOfTransaxialSubmodules * NbOfTransaxialCrystals;
//   fCrystalID += NbOfCrystalsPerLayer * fLayerCrystalID;
//   return fCrystalID;
// }
