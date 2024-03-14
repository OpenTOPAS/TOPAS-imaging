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

#pragma once

#include "SPECTScorer.hh"

class PGScorer : public SPECTScorer {
public:
  PGScorer(TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM,
           TsScoringManager *scM, TsExtensionManager *eM, G4String scorerName,
           G4String quantity, G4String outFileName, G4bool isSubScorer);
  ~PGScorer() = default;
};
