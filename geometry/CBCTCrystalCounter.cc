// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "CBCTCrystalCounter.hh"

CBCTCrystalCounter::CBCTCrystalCounter(int NbOfDetectors, int NbOfZBins, int NbOfXBins) :
    NbOfDetectors(NbOfDetectors), NbOfAxialRows(NbOfZBins), NbOfTransaxialRows(NbOfXBins) {}

void CBCTCrystalCounter::SetCurrentRing(int) {}

void CBCTCrystalCounter::SetCurrentDetector(int detectorID) {
    fDetectorID = detectorID;
}

void CBCTCrystalCounter::SetCurrentDetector(int detectorID, int blockIDY, int blockIDZ) {
    fDetectorID = detectorID;
}

// based on CASToR SPECT standard
int CBCTCrystalCounter::GetCrystalBoxID(int ZBin, int XBin) {
    return fDetectorID * NbOfAxialRows * NbOfTransaxialRows + ZBin * NbOfTransaxialRows + XBin;
}

// based on CASToR SPECT standard
int CBCTCrystalCounter::GetCrystalBoxID(int ZBin, int YBin, int XBin) {
    return fDetectorID * NbOfAxialRows * NbOfTransaxialRows + ZBin * NbOfTransaxialRows + XBin;
}
int CBCTCrystalCounter::GetModuleID(int ModuleIDX, int ModuleIDY, int ModuleIDZ) {}
int CBCTCrystalCounter::GetSubmoduleID(int SubmoduleIDX, int SubmoduleIDY, int SubmoduleIDZ) {}
int CBCTCrystalCounter::GetCollimatorID(int collimatorIDx, int collimatorIDz) {}