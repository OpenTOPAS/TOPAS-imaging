// Scorer for PGScorer
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

#include "PGScorer.hh"

PGScorer::PGScorer(TsParameterManager *pM, TsMaterialManager *mM,
                   TsGeometryManager *gM, TsScoringManager *scM,
                   TsExtensionManager *eM, G4String scorerName,
                   G4String quantity, G4String outFileName, G4bool isSubScorer)
    : SPECTScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName,
                  isSubScorer) {}
