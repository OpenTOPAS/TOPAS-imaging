// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.
#pragma once

#include "CrystalCounter.hh"
#include "globals.hh"
#include <cstring>
#include <fstream>
#include <sstream>

class PETCrystalCounter : public CrystalCounter
{
public:
  PETCrystalCounter() = default;
  ~PETCrystalCounter() = default;
  PETCrystalCounter(int, int, int, int, int, int, int, int, int, int);
  PETCrystalCounter(int, int, int);
  PETCrystalCounter(int, int, int, int, int, int, int);
  void SetCurrentRing(int) override;
  void SetCurrentDetector(int) override;
  void SetCurrentDetector(int, int, int) override;
  void GetCurrentModule(int, int, int);
  int GetCrystalBoxID(int, int) override;
  int GetCrystalBoxID(int, int, int) override;

  void SetDetectorID(int);
  void SetModuleID(int, int, int);
  int GetModuleID(int, int, int);

  int GetSubmoduleID(int, int, int);
  void SetSubmoduleID(int, int, int);

  int GetCrystalID(int, int, int);
  void SetCrystalID(int, int, int);

  int GetCollimatorID(int, int);

protected:
  const int NbOfDetectors;

  const int NbOfAxialModules;
  const int NbOfLayerModules;
  const int NbOfTransaxialModules;
  const int NbOfModulesPerDetector;

  const int NbOfAxialSubmodules;
  const int NbOfLayerSubmodules;
  const int NbOfTransaxialSubmodules;
  const int NbOfSubmodulesPerModule;
  const int NbOfTransaxialSubmodulesPerRSector;
  const int NbOfSubmodulesPerRing;
  const int NbOfSubmodulesPerLayer;
  const int NbOfSubmodulesPerAxialModule;
  const int NbOfSubmodulesPerLayerModule;

  const int NbOfAxialCrystals;
  const int NbOfLayerCrystals;
  const int NbOfTransaxialCrystals;
  const int NbOfTransaxialCrystalPerRSector;
  const int NbOfTransaxialCrystalPerModule;
  const int NbOfCrystalsPerSubmodule;
  const int NbOfCrystalsPerRing;
  const int NbOfCrystalsPerLayer;
  const int NbOfTransaxialCrystalPerAxialSubmodule;
  const int NbOfTransaxialCrystalPerLayerSubmodule;
  const int NbOfTransaxialCrystalPerAxialModule;
  const int NbOfTransaxialCrystalPerLayerModule;

  int NbOfModulesPerRing;
  int NbOfModulesPerLayer;

  int fAxialRingSectorID;
  int fRingSectorID;

  int fAxialModuleID;
  int fTransaxialModuleID;
  int fLayerModuleID;
  int fBlockID;

  int fAxialSubmoduleID;
  int fLayerSubmoduleID;
  int fTransaxialSubmoduleID;
  int fSubmoduleID;

  int fAxialCrystalID;
  int fLayerCrystalID;
  int fTransaxialCrystalID;
  int fCrystalID;
  int fRingID;
};
