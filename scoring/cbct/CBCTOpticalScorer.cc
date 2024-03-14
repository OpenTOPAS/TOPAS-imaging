// Scorer for CBCTOpticalScorer
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

#include "CBCTOpticalScorer.hh"
#include "G4PSDirectionFlag.hh"
CBCTOpticalScorer::CBCTOpticalScorer(TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM, TsScoringManager *scM, TsExtensionManager *eM,
                                     G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer)
    : TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer)
{
    SetSurfaceScorer();
    SetUnit("");
    if (fPm->ParameterExists(GetFullParmName("DetectionEfficiency")))
    {
        fDetectionEfficiency = fPm->GetUnitlessParameter(GetFullParmName("DetectionEfficiency"));
        std::cout << fDetectionEfficiency << std::endl;
    }
    else
    {
        fDetectionEfficiency = 0.05;
        std::cout << "DetectionEfficiency is required" << std::endl;
        // exit(-1);
    }

    if (fPm->ParameterExists(GetFullParmName("DetectorGain")))
    {
        fDetectorGain = fPm->GetUnitlessParameter(GetFullParmName("DetectorGain"));
        std::cout << fDetectorGain << std::endl;
    }
    else
    {
        fDetectorGain = 2.7e5;
        // std::cout << "DetectorGain is required" << std::endl;
        // exit(-1);
    }
    srand(0);
}
CBCTOpticalScorer::~CBCTOpticalScorer() { ; }
G4bool CBCTOpticalScorer::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    if (!fIsActive)
    {
        fSkippedWhileInactive++;
        return false;
    }

    ResolveSolid(aStep);

    if (IsSelectedSurface(aStep))
    {
        G4double weight = aStep->GetPreStepPoint()->GetWeight();

        if (weight != 0. && rand() % 2 < fDetectionEfficiency)
        {
            AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain);
            return true;
        }
    }
    return false;
}