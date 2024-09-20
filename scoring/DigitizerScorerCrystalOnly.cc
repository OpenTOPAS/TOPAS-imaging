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
                          outFileName, isSubScorer) {
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
                          outFileName, isSubScorer) {
    fDetectorName = fPm->GetStringParameter(GetFullParmName("Component"));

    fCrystalLVName = fDetectorName + "/CrLV";

    G4cerr << "The detector is called: " << fDetectorName << G4endl;
    G4cerr << "Crystal logical volume name: " << fCrystalLVName << G4endl;
}

bool DigitizerScorerCrystalOnly::ProcessHits(G4Step *aStep,
                                             G4TouchableHistory *) {
    if (!fIsActive) {
//    G4cout << "Skip" << G4endl;
        fSkippedWhileInactive++;
        return false;
    }

    fEventID = GetEventID();
    ResolveSolid(aStep);
    G4double edep = aStep->GetTotalEnergyDeposit();
    G4Track *aTrack = aStep->GetTrack();

    if (edep > 0) {
        auto currentPV = aStep->GetPostStepPoint()->GetPhysicalVolume();

        if (currentPV == nullptr) {
            return false;
        }

        auto currentLV = currentPV->GetLogicalVolume();

        if (fCrystalBoxLV == nullptr) {
            if (currentLV->GetName() == fCrystalLVName) {
                fCrystalBoxLV = currentLV;
                // G4cout << "currentLV: " << currentLV->GetName() << " fCrystalLVName: " << fCrystalLVName << G4endl;
            } else {
                return false;
            }
        } else if (fCrystalBoxLV != currentLV) {
            return false;
        }
        const auto hitID = std::make_pair(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName(), fEventID);
        auto lowIter = std::lower_bound(
                pulses.begin(), pulses.end(), fEventID,
                [&](std::pair <ID, Pulse> a, G4int b) { return a.first.second < b; });
        if (lowIter == pulses.end() || lowIter->first != hitID) {
            // randomly offset times of histories
            G4double offset = 0;
            G4AutoLock l(&aMutex);
            if (eventOffsets.find(fEventID) != eventOffsets.end()) { // key exists
                offset = eventOffsets[fEventID];
            } else { // key doesn't exist
                offset = fOffsetWindow * G4UniformRand();
                eventOffsets[fEventID] = offset;
            }
            l.unlock();

            pulses.insert(
                    lowIter, std::make_pair(hitID, Pulse(aStep, GetTime() + offset, fProjectionID, fEventID)));
            return true;
        } else {

            lowIter->second.Add(aStep);
            return true;
        }
    } else {
        return false;
    }

    return true;
}
