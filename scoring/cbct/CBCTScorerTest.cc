// Scorer for CBCTScorerTest
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

#include "CBCTScorerTest.hh"
#include "G4PSDirectionFlag.hh"
G4Mutex CBCTScorerTest::aMutex = G4MUTEX_INITIALIZER;

CBCTScorerTest::CBCTScorerTest(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM,
    TsScoringManager* scM, TsExtensionManager* eM, G4String scorerName, G4String quantity,
    G4String outFileName, G4bool isSubScorer) :
    TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer) {
    // SetSurfaceScorer();
    SetUnit("");
}
CBCTScorerTest::~CBCTScorerTest() {
    ;
}

G4bool CBCTScorerTest::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    if (!fIsActive) {
        fSkippedWhileInactive++;
        return false;
    }
    G4Track* aTrack = aStep->GetTrack();
    G4cout << aTrack->GetVolume()->GetName() << G4endl;
    G4double edep = aStep->GetTotalEnergyDeposit();
    ResolveSolid(aStep);
    G4cout << fSolid->GetName() << G4endl;
    if (edep > 0.) {
        ResolveSolid(aStep);
        AccumulateHit(aStep, edep);
        return true;
    }

    // if (IsSelectedSurface(aStep)) {
    //     G4double weight = aStep->GetPreStepPoint()->GetWeight();
    //     if (fForcedInteraction && weight != 0.) {
    //         AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain * fDetectionEfficiency);
    //         aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
    //         return true;
    //     } else {
    //         if (weight != 0. && rand() % 2 < fDetectionEfficiency) {
    //             AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain);
    //             return true;
    //         }
    //     }
    // }

    return false;
}