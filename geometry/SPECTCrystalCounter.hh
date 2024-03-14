// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.
#pragma once
#include "CrystalCounter.hh"

class SPECTCrystalCounter : public CrystalCounter
{
public:
    SPECTCrystalCounter()  = default;
    ~SPECTCrystalCounter() = default;
    SPECTCrystalCounter(int, int, int);
    void SetCurrentRing(int) override;
    void SetCurrentDetector(int) override;
    void SetCurrentDetector(int, int, int) override;
    int  GetCrystalBoxID(int, int) override;
    int  GetCrystalBoxID(int, int, int) override;
    int  GetModuleID(int, int, int) override;
    int  GetSubmoduleID(int, int, int) override;
    int  GetCollimatorID(int, int) override;

protected:
    int NbOfDetectors;
    int NbOfAxialRows;
    int NbOfTransaxialRows;

    int fRingID;
    int fDetectorID;
    int fBlockID;
};
