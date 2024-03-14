// Scorer for DigitizerScorerCrystalOnly
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.
#include "DigitizerScorerCrystalOnly.hh"
#include "Randomize.hh"

DigitizerScorerCrystalOnly::DigitizerScorerCrystalOnly(
    TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM,
    TsScoringManager *scM, TsExtensionManager *eM, G4String scorerName,
    G4String quantity, G4String outFileName, G4bool isSubScorer,
    G4String crystalBoxLVName)
    : DigitizerScorer(true, pM, mM, gM, scM, eM, scorerName, quantity,
                      outFileName, isSubScorer)
{
  fDetectorName = fPm->GetStringParameter(GetFullParmName("Component"));

  fCrystalLVName = fDetectorName + crystalBoxLVName;

  G4cerr << "The detector is called: " << fDetectorName << G4endl;
  G4cerr << "Crystal logical volume name: " << fCrystalLVName << G4endl;
}

DigitizerScorerCrystalOnly::DigitizerScorerCrystalOnly(
    TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM,
    TsScoringManager *scM, TsExtensionManager *eM, G4String scorerName,
    G4String quantity, G4String outFileName, G4bool isSubScorer)
    : DigitizerScorer(true, pM, mM, gM, scM, eM, scorerName, quantity,
                      outFileName, isSubScorer)
{
  fDetectorName = fPm->GetStringParameter(GetFullParmName("Component"));

  fCrystalLVName = fDetectorName + "/CrLV";

  G4cerr << "The detector is called: " << fDetectorName << G4endl;
  G4cerr << "Crystal logical volume name: " << fCrystalLVName << G4endl;
}

bool DigitizerScorerCrystalOnly::ProcessHits(G4Step *aStep,
                                             G4TouchableHistory *)
{
  if (!fIsActive)
  {
    G4cout << "Skip" << G4endl;
    fSkippedWhileInactive++;
    return false;
  }
  // G4cout << "Hit" << G4endl;
  // if(aStep->GetTotalEnergyDeposit()<=0){return false;}

  fEventID = GetEventID();
  ResolveSolid(aStep);
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4Track *aTrack = aStep->GetTrack();

  // if (edep > 0)
  if (true)
  {
    // aTrack->SetTrackStatus(fKillTrackAndSecondaries);

    auto currentPV = aStep->GetPostStepPoint()->GetPhysicalVolume();

    if (currentPV == nullptr)
    {
      return false;
    }

    auto currentLV = currentPV->GetLogicalVolume();

    if (fCrystalBoxLV == nullptr)
    {
      if (currentLV->GetName() == fCrystalLVName)
      {
        fCrystalBoxLV = currentLV;
        // G4cout << "currentLV: " << currentLV->GetName() << " fCrystalLVName: " << fCrystalLVName << G4endl;
      }
      else
      {
        // G4cout << "1 currentLV: " << currentLV->GetName() << aStep->GetTotalEnergyDeposit() << G4endl;
        return false;
      }
    }
    else if (fCrystalBoxLV != currentLV)
    {
      // G4cout << "2 currentLV: " << currentLV->GetName() << " fCrystalLVName: " << fCrystalBoxLV->GetName() << G4endl;
      // G4cout << "2 currentLV: " << currentLV->GetName() <<" "<< aStep->GetTotalEnergyDeposit() << G4endl;

      return false;
    }
    // G4cout << "eventID: " << fEventID << " PV: " << currentPV->GetName() << " LV: " << currentLV->GetName()<< G4endl;
    const auto hitID = std::make_pair(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName(), fEventID);
    auto lowIter = std::lower_bound(
        pulses.begin(), pulses.end(), fEventID,
        [&](std::pair<ID, Pulse> a, G4int b)
        { return a.first.second < b; });
    if (lowIter == pulses.end() || lowIter->first != hitID)
    {

      // G4cout<<"Attach pulse"<<G4endl;

      // randomly offset times of histories
      G4double offset = 0;
      G4AutoLock l(&aMutex);
      if (eventOffsets.find(fEventID) != eventOffsets.end())
      { // key exists
        offset = eventOffsets[fEventID];
        // G4cout << "Offset found " << fEventID << ": " << offset / ns << " ns" << G4endl;
      }
      else
      { // key doesn't exist
        offset = fOffsetWindow * G4UniformRand();
        // G4cout << "Offset not found " << fEventID << ": " << offset / ns << " ns" << G4endl;
        // G4cout << fEventID << ", ";
        eventOffsets[fEventID] = offset;
      }
      l.unlock();
      // G4cout<<"Process hit: edep "<<aStep->GetTotalEnergyDeposit()<<" total E "<<aStep->GetTrack()->GetTotalEnergy()<<G4endl;
      // G4cout<<"Step "<<aStep->GetPostStepPoint()->GetStepStatus()<<G4endl;
      // const G4VProcess* process=aStep->GetPostStepPoint()->GetProcessDefinedStep();
      // process->DumpInfo();
      // G4cout << "global: " << aStep->GetPreStepPoint()->GetGlobalTime() << " local: " << aStep->GetPreStepPoint()->GetLocalTime() << " proper: " << aStep->GetPreStepPoint()->GetProperTime() << G4endl;
      // G4cout << "GetTime: " << GetTime() << G4endl;
      // offset = 0;
      pulses.insert(
          lowIter, std::make_pair(hitID, Pulse(aStep, GetTime() + offset, fProjectionID, fEventID)));
      // lowIter->second.Add(aStep);
      return true;
    }
    else
    {
      // G4cout<<"hits check"<<G4endl;
      // assert(lowIter->first == hitID);
      // G4cout << "Adding new pulse energy: " << lowIter->second.energy << " particle energy " << aStep->GetPreStepPoint()->GetKineticEnergy() << G4endl;
      // G4cout<<"Step "<<aStep->GetPostStepPoint()->GetStepStatus()<<G4endl;
      // assert(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName())
      // G4cout << lowIter->first.first << " " << hitID.first << G4endl;
      lowIter->second.Add(aStep);
      // G4cout << "Adding new pulse energy after add: " << lowIter->second.energy << G4endl;
      return true;
    }
  }
  else
  {
    return false;
  }

  return true;
}
