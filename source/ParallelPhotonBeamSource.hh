// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#pragma once

#include "TsSource.hh"

class ParallelPhotonBeamSource : public TsSource {
public:
  ParallelPhotonBeamSource(TsParameterManager *pM, TsSourceManager *psM,
                           G4String sourceName);
  ~ParallelPhotonBeamSource() = default;

  void ResolveParameters();
};
