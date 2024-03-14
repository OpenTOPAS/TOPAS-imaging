// Extra Class for use by RingDetector
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once

class CrystalCounter
{
public:
    CrystalCounter()                                                                  = default;
    ~CrystalCounter()                                                                 = default;
    virtual void SetCurrentRing(int ringIndex)                                        = 0;
    virtual void SetCurrentDetector(int detectorIndex)                                = 0;
    virtual void SetCurrentDetector(int blockIDX, int blockIDY, int blockIDZ)         = 0;
    virtual int  GetCrystalBoxID(int ZBin, int XBin)                                  = 0;
    virtual int  GetCrystalBoxID(int CrystalIDX, int CrystalIDY, int CrystalIDZ)      = 0;
    virtual int  GetModuleID(int ModuleIDX, int ModuleIDY, int ModuleIDZ)             = 0;
    virtual int  GetSubmoduleID(int SubmoduleIDX, int SubmoduleIDY, int SubmoduleIDZ) = 0;
    virtual int  GetCollimatorID(int collimatorIDx, int collimatorIDz)                = 0;
};
