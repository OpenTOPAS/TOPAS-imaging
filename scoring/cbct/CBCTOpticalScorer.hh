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

#ifndef CBCTOpticalScorer_hh
#define CBCTOpticalScorer_hh

#include "TsVBinnedScorer.hh"
#include <random>
class CBCTOpticalScorer : public TsVBinnedScorer
{
public:
    CBCTOpticalScorer(TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM, TsScoringManager *scM, TsExtensionManager *eM,
                      G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer);

    virtual ~CBCTOpticalScorer();

    G4bool ProcessHits(G4Step *, G4TouchableHistory *);
    G4double fDetectionEfficiency;
    G4double fDetectorGain;
};
#endif