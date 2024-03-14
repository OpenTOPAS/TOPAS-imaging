// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "G4AutoLock.hh"
#include "DigitizerScorer.hh"

class TsParameterManager;

class RotationBeginRun {
public:
	RotationBeginRun(TsParameterManager* pM);
	~RotationBeginRun() = default;

	static std::vector<DigitizerScorer*> scorersToSet;
	static G4Mutex aMutex;
};
