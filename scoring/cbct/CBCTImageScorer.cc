// Scorer for CBCTImageScorer
//
// ********************************************************************
// *                                                                  *
// *                                                                  *
// * This file was obtained from Topas MC Inc under the license       *
// * agreement set forth at http://www.topasmc.org/registration       *
// * Any use of this file constitutes full acceptance of              *
// * this TOPAS MC license agreement.                                 *
// *                                                                  *
// ********************************************************************
//

#include <iostream>

#include "CBCTImageScorer.hh"

CBCTImageScorer::CBCTImageScorer(TsParameterManager *pM, TsMaterialManager *mM,
                       TsGeometryManager *gM, TsScoringManager *scM,
                       TsExtensionManager *eM, G4String scorerName,
                       G4String quantity, G4String outFileName,
                       G4bool isSubScorer)
    : DigitizerScorerCrystalOnly(pM, mM, gM, scM, eM, scorerName, quantity,
                                 outFileName, isSubScorer)
{
  InitializeRotationAngleTracking();
  InitializeOutputColumns();
}

void CBCTImageScorer::InitializeOutputColumns()
{

  G4cout << "CBCTScorer output setup" << G4endl;
  fNtuple->RegisterColumnD(&fXPos, "Y Position", "cm");
  fNtuple->RegisterColumnD(&fYPos, "Y Position", "cm");
  fNtuple->RegisterColumnD(&fZPos, "Z Position", "cm");
  fNtuple->RegisterColumnS(&fVolumeName, "Volume Name");
  fNtuple->RegisterColumnD(&fEnergy, "Energy", "MeV");
  // fNtuple->RegisterColumnD(&fStartGlobalTime, "Pulse Start Time Global", "ns");
  // fNtuple->RegisterColumnD(&fEndGlobalTime, "Pulse End Time Global", "ns");
  // fNtuple->RegisterColumnD(&fTopasTime, "Pulse Time TOPAS", "s");

  // fNtuple->RegisterColumnD(&fRotationPosition, "Rotation Position", "deg");
}

void CBCTImageScorer::FillOutputColumns(std::vector<Pulse> *pulsesSoFar)
{
  for (auto pulse : *pulsesSoFar)
  {
    G4ThreeVector position = pulse.position;
    fXPos = position.x();
    fYPos = position.y();
    fZPos = position.z();
    fEnergy = pulse.energy;
    fVolumeName = pulse.physicalVolumeName;
    fStartGlobalTime = pulse.startTimeGlobal;
    fEndGlobalTime = pulse.endTimeGlobal;
    fTopasTime = pulse.eventStartTimeTopas;
    fRotationPosition = fRotationAngles[pulse.projectionId];
    // G4cout << "fYpos " << fYPos << " "
    //        << "fZPos " << fZPos << " "
    //        << "fEnergy " << fEnergy << " "
    //        << "fStartGlobalTime " << fStartGlobalTime << " "
    //        << "fEndGlobalTime " << fEndGlobalTime << G4endl;
    if (fEnergy > 0)
    {
      fNtuple->Fill();
    }
    // fNtuple->Fill();
  }
}

// NOTE: assumes all digitizers only act on a single pulse
// TODO: generalize to fix this assumption
void CBCTImageScorer::ApplyDigitizers()
{
  // sort pulses by end times
  // G4cout << "Apply in CBCT" << G4endl;
  std::sort(addedPulses->begin(), addedPulses->end(),
            [&](Pulse pulse1, Pulse pulse2) {
              return pulse1.endTimeTopas < pulse2.endTimeTopas;
            });

  std::vector<Pulse> pulsesSoFar = *addedPulses;

  for (auto &digitizer : *digitizers)
  {
    // G4cout << "digitier: " << G4endl;
    std::vector<Pulse> newPulses;
    digitizer->ProcessPulses(pulsesSoFar, newPulses);
    // set up for next in chain
    pulsesSoFar = newPulses;
  }

  for (int i = 0; i < pulsesSoFar.size(); i++)
  {
    pulsesSoFar[i].beenDigitized = true;
  }

  *addedPulses = pulsesSoFar;
}
