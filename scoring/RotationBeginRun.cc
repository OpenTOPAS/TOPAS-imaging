// BeginRun for TOPAS
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "SPECTScorer.hh"
#include "RotationBeginRun.hh"

#include "TsParameterManager.hh"

G4Mutex RotationBeginRun::aMutex = G4MUTEX_INITIALIZER;
std::vector<DigitizerScorer*> RotationBeginRun::scorersToSet = std::vector<DigitizerScorer*>();

//This class helps keep track of how the detector is rotated around the center of the FOV. This is useful for SPECT
RotationBeginRun::RotationBeginRun(TsParameterManager* pM) {
	G4AutoLock l(&aMutex);
	for(auto scorer : scorersToSet) {
		scorer->RegisterRotationAngles();
	}
	l.unlock();
}
