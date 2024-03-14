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

#include "DigitizerScorerCrystalOnly.hh"

class CBCTImageScorer : public DigitizerScorerCrystalOnly
{
public:
  CBCTImageScorer(TsParameterManager *pM, TsMaterialManager *mM,
             TsGeometryManager *gM, TsScoringManager *scM,
             TsExtensionManager *eM, G4String scorerName, G4String quantity,
             G4String outFileName, G4bool isSubScorer);

  ~CBCTImageScorer() = default;

  G4String fVolumeName;
  void InitializeOutputColumns() override;
  void FillOutputColumns(std::vector<Pulse> *pulsesSoFar) override;
  void ApplyDigitizers() override;

protected:
  // Output variables
  G4double fRotationPosition;
};
