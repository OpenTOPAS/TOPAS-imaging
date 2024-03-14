// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "DeadTime.hh"
#include "globals.hh"

#include <math.h>

G4Mutex DeadTime::aMutex = G4MUTEX_INITIALIZER;

DeadTime::DeadTime(G4double dead_time) : dead_time(dead_time) {}

void DeadTime::ProcessPulse(Pulse pulse,
                            std::vector<Pulse> &pulses)
{
}

void DeadTime::ProcessPulses(std::vector<Pulse> pulses,
                             std::vector<Pulse> &newPulses)
{
  G4double current_time = 0, last_time = 0;
  G4int crystalIndex = 0;
  G4AutoLock l(&aMutex);
  std::map<G4int, G4double> time_map = std::map<G4int, G4double>();
  for (auto pulse : pulses)
  {
    assert(pulse.endTimeTopas > current_time);
    crystalIndex = pulse.crystalIndex;
    // G4cout << "pulse endtime: " << pulse.endTimeTopas << " current time: " << current_time << " deadtime: " << dead_time << G4endl;
    if (time_map.find(crystalIndex) != time_map.end())
    {
      last_time = time_map[crystalIndex];
      // G4cout << "Found: " << crystalIndex << " " << last_time << " " << pulse.endTimeTopas - last_time << G4endl;

      if (pulse.endTimeTopas - last_time > dead_time)
      {
        // G4cout << "Updating: " << crystalIndex << " " << pulse.endTimeTopas << G4endl;
        time_map[crystalIndex] = pulse.endTimeTopas;
        newPulses.push_back(pulse);
      }
    }
    else
    {
      // G4cout << "Adding: " << crystalIndex << " " << pulse.endTimeTopas << G4endl;
      time_map[crystalIndex] = pulse.endTimeTopas;
      newPulses.push_back(pulse);
    }
  }
  l.unlock();
}
