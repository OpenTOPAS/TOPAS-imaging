// Component for XRayTube
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

#include "G4Colour.hh"
#include "TsVGeometryComponent.hh"

class XRayTube : public TsVGeometryComponent {
public:
  XRayTube(TsParameterManager *pM, TsExtensionManager *eM,
           TsMaterialManager *mM, TsGeometryManager *gM,
           TsVGeometryComponent *parentComponent,
           G4VPhysicalVolume *parentVolume, G4String &name);

  ~XRayTube() = default;
  G4VPhysicalVolume *Construct();
  
protected:
  G4double fRMin;
  G4double fRMid;
  G4double fRMax;
  G4double fLength;
  G4double fHousingRmin;
  G4String fInnerHouseMaterial;
  G4String fOuterHouseMaterial;
	G4double fSockRMin;
  G4double fSockRMax;
	G4double fSockHeight;
	G4double fHRMin;
	G4double fHRMax;
	G4double fHHeight;
  void ColorLV(G4double, G4double, G4double, G4LogicalVolume *);
  void ColorLV(G4Colour, G4LogicalVolume *);
};
