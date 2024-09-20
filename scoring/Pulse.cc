// Extra Class for use by DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "Pulse.hh"

Pulse::Pulse(Pulse original, G4double timeShift, G4double energyShift, G4ThreeVector positionShift) :
    physicalVolumeName(original.physicalVolumeName), energy(original.energy + energyShift),
    endTimeGlobal(timeShift + original.endTimeGlobal), endTimeTopas(timeShift + original.endTimeTopas),
    startTimeGlobal(timeShift + original.startTimeGlobal), startTimeTopas(timeShift + original.startTimeTopas),
    // unchanged on purpose since the start topas time should stay the same
    // for all events in the same run
    eventStartTimeTopas(original.eventStartTimeTopas), position(original.position + positionShift),
    crystalIndex(original.crystalIndex), projectionId(original.projectionId), beenDigitized(false),
    eventID(original.eventID), OriginPosition(original.OriginPosition), submoduleIndex(original.submoduleIndex),
    moduleIndex(original.moduleIndex), detectorIndex(original.detectorIndex), historyDepth(original.historyDepth) {}

Pulse::Pulse(Pulse first, Pulse second) :
    physicalVolumeName(first.physicalVolumeName), energy(first.energy + second.energy), position(first.position),
    crystalIndex(first.crystalIndex), projectionId(first.projectionId), beenDigitized(false), eventID(first.eventID),
    OriginPosition(first.OriginPosition), submoduleIndex(first.submoduleIndex), moduleIndex(first.moduleIndex),
    detectorIndex(first.detectorIndex), historyDepth(first.historyDepth) {

    // take later end time and earlier start time
    if (second.endTimeTopas > first.endTimeTopas) {
        endTimeGlobal = second.endTimeGlobal;
        endTimeTopas  = second.endTimeTopas;
    } else {
        endTimeGlobal = first.endTimeGlobal;
        endTimeTopas  = first.endTimeTopas;
    }

    if (second.startTimeTopas < first.startTimeTopas) {
        startTimeGlobal = second.startTimeGlobal;
        startTimeTopas  = second.startTimeTopas;
    } else {
        startTimeGlobal = first.startTimeGlobal;
        startTimeTopas  = first.startTimeTopas;
    }

    eventStartTimeTopas = std::min(second.eventStartTimeTopas, first.eventStartTimeTopas);
}

Pulse::Pulse(G4Step* aStep, G4double eventStartTimeTopas_, G4int projectionId_, G4int eventID_) :
    eventStartTimeTopas(eventStartTimeTopas_),   // energy(0),
    projectionId(projectionId_), beenDigitized(false), eventID(eventID_) {
    // getting position of crystal
    // code taken from here:
    // https://geant4-forum.web.cern.ch/t/retrieve-the-position-in-global-x-y-z-coordinates-of-a-hit-sensitive-volume/1537
    //    G4TouchableHandle theTouchable = aStep->GetPostStepPoint()->GetTouchableHandle();
    G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector     origin(0., 0., 0.);
    G4ThreeVector     globalOrigin = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(origin);

    position = G4ThreeVector(
        0., 0., 0.);   // = aStep->GetPostStepPoint()->GetPosition(); // position; //theTouchable->GetPosition();

    startTimeGlobal = aStep->GetPreStepPoint()->GetGlobalTime();

    startTimeTopas = startTimeGlobal + eventStartTimeTopas;

    //    physicalVolumeName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
    physicalVolumeName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();

    historyDepth   = theTouchable->GetHistory()->GetDepth();
    crystalIndex   = theTouchable->GetHistory()->GetTopVolume()->GetCopyNo();
    submoduleIndex = theTouchable->GetHistory()->GetVolume(historyDepth - 1)->GetCopyNo();
    moduleIndex    = theTouchable->GetHistory()->GetVolume(historyDepth - 2)->GetCopyNo();
    detectorIndex  = theTouchable->GetHistory()->GetVolume(historyDepth - 3)->GetCopyNo();
    // G4cout << "Crystal " << theTouchable->GetHistory()->GetTopVolume()->GetName() << G4endl;
    // G4cout << "Submodule " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 1)->GetName() << " " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 1)->GetCopyNo() << G4endl;
    // G4cout << "Module " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 2)->GetName() << " " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 2)->GetCopyNo() << G4endl;
    // G4cout << "Detector " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 3)->GetName() << " " << theTouchable->GetHistory()->GetVolume(theTouchable->GetHistory()->GetDepth() - 3)->GetCopyNo() << G4endl;
    // for (int llll = 0; llll < theTouchable->GetHistory()->GetDepth(); llll++)
    // {
    //   G4cout << "scorer: " << llll << " " << theTouchable->GetHistory()->GetVolume(llll)->GetName() << G4endl;
    // }
    //   G4cout << "scorer: " << llll << " " << theTouchable->GetHistory()->BackLevel()->GetName() << G4endl;

    energy = 0;
    //    endTimeGlobal                         = aStep->GetPostStepPoint()->GetGlobalTime();
    endTimeGlobal                         = aStep->GetPreStepPoint()->GetGlobalTime();
    endTimeTopas                          = endTimeGlobal + eventStartTimeTopas;
    TsTrackInformation* parentInformation = (TsTrackInformation*) (aStep->GetTrack()->GetUserInformation());

    std::vector<const G4Track*>        tracks    = parentInformation->GetParentTracks();
    std::vector<G4ParticleDefinition*> particles = parentInformation->GetParticleDefs();
    // std::vector<G4VProcess *> process = parentInformation->GetCreatorProcesses();
    std::vector<G4ThreeVector> vertex          = parentInformation->GetParentTrackVertexPositions();
    G4ParticleDefinition*      GammaDefinition = G4ParticleTable::GetParticleTable()->FindParticle("gamma");

    G4int first_gamma = -1;
    if (tracks.size() > 0 && particles.size() > 0) {
        // G4cout << particles.size() << " " << tracks.size() << G4endl;
        for (int parents_ind = tracks.size() - 1; parents_ind >= 0; parents_ind--) {
            if (particles[parents_ind] == GammaDefinition) {
                first_gamma = parents_ind;
                break;
            }
            // G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << particles[iiii]->GetParticleName() << " " << tracks[iiii]->GetProperTime() << " " << tracks[iiii]->GetPosition() << " " << tracks[iiii]->GetParentID() << " " << tracks[iiii]->GetTrackID() << G4endl;
            // G4cout << "vertex " << vertex[parents_ind] << G4endl;
        }
        if (first_gamma == -1) {
            OriginPosition = aStep->GetTrack()->GetVertexPosition();
        } else {
            OriginPosition = vertex[first_gamma];
        }
        // assert(particles.back()->GetParticleName() == "e+");
        // creation_time = 0; // tracks.back()->GetProperTime();
        // G4TrackVector *secondaries = tracks.back()->GetStep()->GetfSecondary();
        // G4cout << aStep->GetTrack()->GetParentID() << " " << aStep->GetTrack()->GetTrackID() << " " << eventID << " " << OriginPosition << " " << first_gamma << G4endl;
    } else {
        // G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;
        OriginPosition = G4ThreeVector(0., 0., 0.);
        // creation_time = 0;
    }

    Add(aStep);
}

void Pulse::Add(G4Step* aStep) {
    // set new end time
    // endTimeGlobal = aStep->GetPostStepPoint()->GetGlobalTime();
    // endTimeTopas = endTimeGlobal + eventStartTimeTopas;
    // G4cout<<"total energy deposit "<<aStep->GetTotalEnergyDeposit()<<G4endl;
    // G4Track *aTrack = aStep->GetTrack();
    // G4cout<<"kinetic energy "<<aTrack->GetKineticEnergy()<<G4endl;
    // add energy

    // if(aStep->GetTotalEnergyDeposit()>0){
    //   energy += aStep->GetTotalEnergyDeposit();
    // }
    G4double edep = aStep->GetTotalEnergyDeposit();
//    position += aStep->GetPostStepPoint()->GetPosition() * edep;
    position += aStep->GetPreStepPoint()->GetPosition() * edep;
    energy += edep;

    // position = aStep->GetPostStepPoint()->GetPosition() ;
    // energy += aStep->GetPreStepPoint()->GetKineticEnergy();
    // G4cout << "Energy check" << G4endl;
    // assert(aStep->GetTotalEnergyDeposit() > 0.0);
    // G4cout<<"add pulse energy: "<<energy<<" edep "<<aStep->GetTotalEnergyDeposit()<<G4endl;
}
