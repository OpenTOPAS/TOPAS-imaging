// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "SPECTCrystalCounter.hh"

SPECTCrystalCounter::SPECTCrystalCounter(int NbOfDetectors, int NbOfZBins, int NbOfXBins) :
    NbOfDetectors(NbOfDetectors), NbOfAxialRows(NbOfZBins), NbOfTransaxialRows(NbOfXBins) {}

void SPECTCrystalCounter::SetCurrentRing(int) {}

void SPECTCrystalCounter::SetCurrentDetector(int detectorID) {
    fDetectorID = detectorID;
}

void SPECTCrystalCounter::SetCurrentDetector(int detectorID, int blockIDY, int blockIDZ) {
    fDetectorID = detectorID;
}

// based on CASToR SPECT standard
int SPECTCrystalCounter::GetCrystalBoxID(int ZBin, int XBin) {
    return fDetectorID * NbOfAxialRows * NbOfTransaxialRows + ZBin * NbOfTransaxialRows + XBin;
}

// based on CASToR SPECT standard
int SPECTCrystalCounter::GetCrystalBoxID(int ZBin, int YBin, int XBin) {
    return fDetectorID * NbOfAxialRows * NbOfTransaxialRows + ZBin * NbOfTransaxialRows + XBin;
}
int SPECTCrystalCounter::GetModuleID(int ModuleIDX, int ModuleIDY, int ModuleIDZ) {}
int SPECTCrystalCounter::GetSubmoduleID(int SubmoduleIDX, int SubmoduleIDY, int SubmoduleIDZ) {}
int SPECTCrystalCounter::GetCollimatorID(int collimatorIDx, int collimatorIDz) {}
