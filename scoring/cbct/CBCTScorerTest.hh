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

#ifndef CBCTScorerTest_hh
#define CBCTScorerTest_hh

#include "G4AutoLock.hh"
#include "G4Threading.hh"
#include "Randomize.hh"
#include "TsVBinnedScorer.hh"
#include <random>
// #include <filesystem>
#include <dirent.h>
#include <glob.h>
class CBCTScorerTest : public TsVBinnedScorer
{
public:
    CBCTScorerTest(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM,
        TsScoringManager* scM, TsExtensionManager* eM, G4String scorerName, G4String quantity,
        G4String outFileName, G4bool isSubScorer);

    virtual ~CBCTScorerTest();
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    // void UserHookForEndOfRun();
    G4bool   fOSF;
    G4double fDetectionEfficiency;
    G4double fDetectorGain;
    G4String OSFPath;
    G4int    index;
    G4int    index2;
    G4int    xBin;
    G4int    yBin;
    G4int    xBinNum;
    G4int    yBinNum;

    G4int                               kernel_size_u;
    G4int                               kernel_size_v;
    G4double                            ParticleEnergy;
    G4double                            Rand;
    G4double                            E0;
    G4double                            E_step;
    std::map<G4int, G4double>           Efficiency;
    std::map<G4int, float**>            kernels;
    std::map<G4int, G4double>::iterator ef_low, ef_up;
    std::map<G4int, float**>::iterator  k_low, k_up;
    float**                             kernel;
    float**                             rkernel;
    float                               weight1;
    float                               weight2;
    float **                            kernel1, **kernel2;

    G4bool    fForcedInteraction;
    float     detection_efficiency;
    G4int     fNumberOfEfficiency;
    G4double* fEfficiencyEnergy;
    G4double* fEfficiency;

    void           ReadNumBins();
    void           ReadPixelSpacings();
    void           ReadOSFPath();
    void           ReadOSFData();
    static G4Mutex aMutex;
};
#endif
