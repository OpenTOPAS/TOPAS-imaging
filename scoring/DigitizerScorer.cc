// Scorer for DigitizerScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include <iostream>

#include "DigitizerScorer.hh"
#include "EnergyCutoffModule.hh"
#include "TimeResolution.hh"

#include "G4PSDirectionFlag.hh"
#include "Randomize.hh"
#include "RotationBeginRun.hh"
#include "globals.hh"

#include <utility>
#include <vector>

#define dbg(v) G4cerr << #v << " = " << v << G4endl

const G4double eps = 1e-9;

using std::ostream;

G4Mutex                                DigitizerScorer::aMutex           = G4MUTEX_INITIALIZER;
std::map<G4String, std::vector<Pulse>> DigitizerScorer::addedPulsesTotal = std::map<G4String, std::vector<Pulse>>();
std::map<G4String, std::vector<DigitizerModule*>> DigitizerScorer::digitizersTotal =
    std::map<G4String, std::vector<DigitizerModule*>>();
std::map<G4int, G4double> DigitizerScorer::eventOffsets = std::map<G4int, G4double>();

DigitizerScorer::DigitizerScorer(G4bool isInherited, TsParameterManager* pM, TsMaterialManager* mM,
    TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM, G4String scorerName, G4String quantity,
    G4String outFileName, G4bool isSubScorer) :
    TsVNtupleScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer) {
    if (!isInherited) {
        InitializeOutputColumns();
    }

    fScorerName = scorerName;

    G4AutoLock l(&aMutex);
    // check if scorer name in dict of pulse vectors
    if (!addedPulsesTotal.count(fScorerName)) {
        addedPulsesTotal[fScorerName] = std::vector<Pulse>();
        G4cerr << "Making pulse vector for " << fScorerName << G4endl;
    }
    G4cerr << "Registering pulse vector for " << fScorerName << G4endl;
    addedPulses = &addedPulsesTotal[fScorerName];

    // register digitizer list
    if (!digitizersTotal.count(fScorerName)) {
        digitizersTotal[fScorerName] = std::vector<DigitizerModule*>();
        G4cerr << "Making digitizer vector for " << fScorerName << G4endl;
    }
    G4cerr << "Registering digitizer vector for " << fScorerName << G4endl;
    digitizers = &digitizersTotal[fScorerName];

    l.unlock();

    fNbRuns              = fPm->GetIntegerParameter("Tf/NumberOfSequentialTimes");
    fIncludeCastorOutput = fPm->ParameterExists(GetFullParmName("EnableCastorOutput"))
                               ? fPm->GetBooleanParameter(GetFullParmName("EnableCastorOutput"))
                               : false;

    if (fIncludeCastorOutput) {
        fCastorScannerName       = fPm->GetStringParameter(GetFullParmName("CastorScannerName"));
        fCastorOutputFilename    = fPm->GetStringParameter(GetFullParmName("CastorOutputFilename"));
        fCastorCalibrationFactor = fPm->ParameterExists(GetFullParmName("CastorCalibrationFactor"))
                                       ? fPm->IGetUnitlessParameter(GetFullParmName("CastorCalibrationFactor"))
                                       : 100000000;
        fCastorIsotope           = fPm->ParameterExists(GetFullParmName("CastorIsotope"))
                                       ? fPm->GetStringParameter(GetFullParmName("CastorIsotope"))
                                       : "";
    }

    fOffsetWindow = fPm->ParameterExists(GetFullParmName("ArtificialOffsetWindow"))
                        ? fPm->GetDoubleParameter(GetFullParmName("ArtificialOffsetWindow"), "Time")
                        : 0;

    fProjectionID = 0;

    G4cout << "Type: " << fOutFileType << G4endl;
    G4cout << "Mode: " << fOutFileMode << G4endl;
    G4cout << "Name: " << fOutFileName << G4endl;

    runCount = 0;
}

DigitizerScorer::DigitizerScorer(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM,
    TsScoringManager* scM, TsExtensionManager* eM, G4String scorerName, G4String quantity, G4String outFileName,
    G4bool isSubScorer) :
    DigitizerScorer(false, pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer) {
    ;
}

DigitizerScorer::~DigitizerScorer() {
    ;
}

void DigitizerScorer::InitializeOutputColumns() {

    G4cerr << "DigitizerScorer output setup" << G4endl;

    fNtuple->RegisterColumnD(&fEnergy, "Energy", "MeV");
    fNtuple->RegisterColumnD(&fStartGlobalTime, "Pulse Start Time Global", "ns");
    fNtuple->RegisterColumnD(&fEndGlobalTime, "Pulse End Time Global", "ns");
    fNtuple->RegisterColumnD(&fTopasTime, "Pulse Time TOPAS", "s");
    fNtuple->RegisterColumnD(&fXPos, "X Position", "cm");
    fNtuple->RegisterColumnD(&fYPos, "Y Position", "cm");
    fNtuple->RegisterColumnD(&fZPos, "Z Position", "cm");
}

void DigitizerScorer::FillOutputColumns(std::vector<Pulse>* pulsesSoFar) {
    for (auto pulse : *pulsesSoFar) {
        G4ThreeVector position = pulse.position;
        fXPos                  = position.x();
        fYPos                  = position.y();
        fZPos                  = position.z();
        fEnergy                = pulse.energy;
        fStartGlobalTime       = pulse.startTimeGlobal;
        fEndGlobalTime         = pulse.endTimeGlobal;
        fTopasTime             = pulse.eventStartTimeTopas;

        fNtuple->Fill();
    }
}

void DigitizerScorer::UserHookForEndOfRun() {
    G4cout << "Digitizer EndofRun" << G4endl;
    G4AutoLock l(&aMutex);

    // runCount++;
    if (addedPulses->size() > 0) {
        G4cout << "Pulse size: " << addedPulses->size() << G4endl;
        PositionCorrection();
        G4cout << "CONSOLIDATING AT END" << G4endl;
        ApplyDigitizers();
        G4cout << "Filloutput columns" << G4endl;
        FillOutputColumns(addedPulses);
        G4cout << "Filloutput columns end" << G4endl;
    }
    // G4cout << "CONSOLIDATING AT END" << G4endl;
    // ApplyDigitizers();
    // G4cout << "Filloutput columns" << G4endl;
    // FillOutputColumns(addedPulses);
    // G4cout << "Filloutput columns end" << G4endl;
    addedPulses->clear();
    eventOffsets.clear();

    // G4cout << "Pulse size: " << addedPulses->size() << G4endl;

    // G4cout << "CONSOLIDATING AT END" << G4endl;
    // G4cout << "addedPulse size: " << addedPulses->size() << G4endl;
    // ApplyDigitizers();
    // FillOutputColumns(addedPulses);
    // addedPulses->clear();
    // G4cout << "addedPulse after save size: " << addedPulses->size() << G4endl;
    l.unlock();
}
void DigitizerScorer::PositionCorrection() {
    G4cout << "PositionCorrection" << G4endl;
    std::vector<Pulse> pulsesSoFar = *addedPulses;
    for (int i = 0; i < pulsesSoFar.size(); i++) {
        pulsesSoFar[i].position /= pulsesSoFar[i].energy;
    }
    *addedPulses = pulsesSoFar;
    G4cout << "End PositionCorrection" << G4endl;
}
// NOTE: assumes all digitizers only act on a single pulse
// TODO: generalize to fix this assumption
void DigitizerScorer::ApplyDigitizers() {
    G4cout << "ApplyDigitizer" << G4endl;
    // sort pulses by end times
    std::sort(addedPulses->begin(), addedPulses->end(), [&](Pulse pulse1, Pulse pulse2) {
        return pulse1.endTimeTopas < pulse2.endTimeTopas;
    });

    std::vector<Pulse> pulsesSoFar = *addedPulses;
    G4cout << "ProcessPulses" << G4endl;
    G4cout << pulsesSoFar.size() << G4endl;
    G4int digit_count = 0;
    for (auto& digitizer : *digitizers) {
        G4cout << "Digitizer count " << digit_count << G4endl;
        G4cout << "Pulse input " << pulsesSoFar.size() << G4endl;
        std::vector<Pulse> newPulses;
        digitizer->ProcessPulses(pulsesSoFar, newPulses);
        // set up for next in chain
        pulsesSoFar = newPulses;
        digit_count += 1;
        G4cout << "Pulse output " << pulsesSoFar.size() << G4endl;
    }
    G4cout << "End ApplyDigitizer" << G4endl;

    for (int i = 0; i < pulsesSoFar.size(); i++) {
        pulsesSoFar[i].beenDigitized = true;
    }

    *addedPulses = pulsesSoFar;
    G4cout << "End ApplyDigitizer" << G4endl;
}

void DigitizerScorer::UserHookForEndOfEvent() {
    for (auto it : pulses) {
        Pulse      pulse = it.second;
        G4AutoLock l(&aMutex);
        addedPulses->push_back(pulse);
        l.unlock();
    }
    pulses.clear();
}

G4bool DigitizerScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    if (!fIsActive) {
        fSkippedWhileInactive++;
        return false;
    }

    fEventID = GetEventID();
    ResolveSolid(aStep);
    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep == 0) {
        return false;
    }
    // G4cout << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    const auto hitID   = std::make_pair(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName(), fEventID);
    auto       lowIter = std::lower_bound(
        pulses.begin(), pulses.end(), hitID, [&](std::pair<ID, Pulse> a, ID b) { return a.first < b; });
    if (lowIter == pulses.end() || lowIter->first != hitID) {

        // randomly offset times of histories
        G4double   offset = 0;
        G4AutoLock l(&aMutex);
        if (eventOffsets.find(fEventID) != eventOffsets.end()) {   // key exists
            offset = eventOffsets[fEventID];
        } else {   // key doesn't exist
            offset = fOffsetWindow * G4UniformRand();
            // G4cout << fEventID << ": " << offset /s << " s" << G4endl;
            eventOffsets[fEventID] = offset;
        }
        l.unlock();

        pulses.insert(lowIter, std::make_pair(hitID, Pulse(aStep, GetTime() + offset, fProjectionID, fEventID)));
    } else {
        lowIter->second.Add(aStep);
    }

    return true;
    // return false;
}

void DigitizerScorer::InitializeRotationAngleTracking() {
    G4AutoLock l(&RotationBeginRun::aMutex);

    RotationBeginRun::scorersToSet.push_back(this);

    l.unlock();
}

void DigitizerScorer::RegisterRotationAngles() {
    G4cerr << "Registering rotation angle" << G4endl;

    G4double rotationAngle = fPm->GetDoubleParameter(GetFullParmName("RotationAngle"), "Angle");

    // put rotationAngle into range of [0,360) degrees
    while (rotationAngle < 0) {
        rotationAngle += 360 * deg;
    }
    while (rotationAngle >= 360 * deg) {
        rotationAngle -= 360 * deg;
    }

    if (fRotationAngles.size() == 0 || abs(fRotationAngles.back() - rotationAngle) > eps) {
        fRotationAngles.push_back(rotationAngle);
        fProjectionID = fRotationAngles.size() - 1;
    }
}
