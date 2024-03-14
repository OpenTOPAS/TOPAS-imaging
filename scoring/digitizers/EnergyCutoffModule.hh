// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once

#include "DigitizerModule.hh"
#include "Pulse.hh"

class EnergyCutoffModule : public DigitizerModule
{
public:
    EnergyCutoffModule(G4double minEnergyWindow, G4double maxEnergyWindow);
    ~EnergyCutoffModule() = default;

protected:
    void ProcessPulse(Pulse, std::vector<Pulse>&) override;

private:
    G4double minEnergy;
    G4double maxEnergy;
};
