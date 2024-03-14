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

#include "XRayTube.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiUnion.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "TsParameterManager.hh"
#include "globals.hh"

#include <cmath>

#define BLUE_COLOR G4Colour(102 / 255., 204 / 255., 255 / 255.)

XRayTube::XRayTube(TsParameterManager *pM, TsExtensionManager *eM,
                   TsMaterialManager *mM, TsGeometryManager *gM,
                   TsVGeometryComponent *parentComponent,
                   G4VPhysicalVolume *parentVolume, G4String &name)
    : TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume,
                           name) {
  ;
}

void XRayTube::ColorLV(G4Colour color, G4LogicalVolume *LV) {
  G4VisAttributes *yokeColor = new G4VisAttributes(color);
  RegisterVisAtt(yokeColor);
  LV->SetVisAttributes(yokeColor);
}

void XRayTube::ColorLV(G4double r, G4double g, G4double b,
                       G4LogicalVolume *LV) {
  ColorLV(G4Colour(r, g, b), LV);
}

G4VPhysicalVolume *XRayTube::Construct(void) {
  BeginConstruction();

  G4cerr << "Starting housing construction" << fName << G4endl;
  fRMin = fPm->GetDoubleParameter(GetFullParmName("RMin"), "Length");
  fRMax = fPm->GetDoubleParameter(GetFullParmName("RMax"), "Length");
  fLength = fPm->GetDoubleParameter(GetFullParmName("Length"), "Length");
	fSockRMin = fPm->GetDoubleParameter(GetFullParmName("SockRMin"), "Length");
  fSockRMax = fPm->GetDoubleParameter(GetFullParmName("SockRMax"), "Length");
  fSockHeight = fPm->GetDoubleParameter(GetFullParmName("SockHeight"), "Length");
	fHRMin = fPm->GetDoubleParameter(GetFullParmName("HRMin"), "Length");
  fHRMax = fPm->GetDoubleParameter(GetFullParmName("HRMax"), "Length");
  fHHeight = fPm->GetDoubleParameter(GetFullParmName("HHeight"), "Length");
  
   auto XRayTubeSd = new G4Tubs("XRTbSd", 1000, 10000, 1000, 0, 2 * M_PI);
  fEnvelopeLog = CreateLogicalVolume("XRTbLV", XRayTubeSd);
  fEnvelopePhys = CreatePhysicalVolume(fEnvelopeLog);

  G4VSolid *HousingSolid =
      new G4Tubs("OuHsSd", fRMin, fRMax, fLength, 0, 2 * M_PI);
  G4LogicalVolume *HousingLV = CreateLogicalVolume(HousingSolid);
  ColorLV(BLUE_COLOR, HousingLV);

  G4Tubs *solidsocket =
      new G4Tubs("solidsocket2", fSockRMin, fSockRMax, fSockHeight, 0.0, 2 * M_PI);
  G4LogicalVolume *socketLV = CreateLogicalVolume(solidsocket);
  ColorLV(BLUE_COLOR, socketLV);

  G4Tubs *hole = new G4Tubs("hole", fHRMin, fHRMax, fHHeight, 0, 2 * M_PI);
  G4LogicalVolume *holeLV = CreateLogicalVolume(hole);

  // placement and assembly
  G4RotationMatrix *rotm = new G4RotationMatrix();
  G4ThreeVector *position1 = new G4ThreeVector(0., 0., 0.); // housing outer
  G4ThreeVector *position2 = new G4ThreeVector(0., 0., 0.); // housing inner
  G4ThreeVector *position3 =
      new G4ThreeVector(fLength * .25, fRMax * .5, 0.); // socket 1
  G4ThreeVector *position4 =
      new G4ThreeVector(-fLength * .25, fRMax * .5, 0.); // socket 2
  G4ThreeVector *position5 =
      new G4ThreeVector(1.62, -fRMax * .5, 20.); // MTG hole 1
  G4ThreeVector *position6 =
      new G4ThreeVector(-1.62, -fRMax * .5, 20.); // MTG hole 2

  CreatePhysicalVolume("OuHsLV", HousingLV, rotm, position1, fEnvelopePhys);
  CreatePhysicalVolume("InHsLV", HousingLV, rotm, position2, fEnvelopePhys);
  CreatePhysicalVolume("Sc1LV", socketLV, rotm, position3, fEnvelopePhys);
  CreatePhysicalVolume("Sc2LV", socketLV, rotm, position4, fEnvelopePhys);
  CreatePhysicalVolume("Hl1LV", holeLV, rotm, position5, fEnvelopePhys);
  CreatePhysicalVolume("Hl2LV", holeLV, rotm, position6, fEnvelopePhys);

  InstantiateChildren(fEnvelopePhys);

  return fEnvelopePhys;
}
