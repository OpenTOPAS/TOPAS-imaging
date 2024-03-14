// Particle Source for ParallelPhotonBeamSource
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "ParallelPhotonBeamSource.hh"

#include "TsParameterManager.hh"

ParallelPhotonBeamSource::ParallelPhotonBeamSource(TsParameterManager *pM,
                                                   TsSourceManager *psM,
                                                   G4String sourceName)
    : TsSource(pM, psM, sourceName) {
  ResolveParameters();
}

void ParallelPhotonBeamSource::ResolveParameters() {
  TsSource::ResolveParameters();
}
