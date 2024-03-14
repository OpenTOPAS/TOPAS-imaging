// Component for FlatImager

// Copyright 2021 Hoyeon Lee (hlee80@mgh.harvard.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "FlatImager.hh"
#include "G4GeometryTolerance.hh"
#include "G4Material.hh"

#include "TsParameterManager.hh"

#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

const G4double eps = 1e-9;

#define dbg(x) G4cerr << #x << " = " << x << G4endl

#ifdef TOPAS_MT
namespace
{
G4Mutex ParameterizationErrorMutex = G4MUTEX_INITIALIZER;
G4Mutex IndexErrorMutex            = G4MUTEX_INITIALIZER;
}   // namespace
#endif

FlatImager::FlatImager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM,
    TsGeometryManager* gM, TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume,
    G4String& name) :
    Imager(pM, eM, mM, gM, parentComponent, parentVolume, name) {

    fDivisionNames[0] = "X";
    fDivisionNames[1] = "Y";
    fDivisionNames[2] = "Z";
    fDivisionUnits[0] = "cm";
    fDivisionUnits[1] = "cm";
    fDivisionUnits[2] = "cm";
    fIsDividable      = true;
}

FlatImager::~FlatImager() {
    ;
}

TsVGeometryComponent::SurfaceType FlatImager::GetSurfaceID(G4String surfaceName) {
    SurfaceType surfaceID;
    G4String    surfaceNameLower = surfaceName;
    surfaceNameLower.toLower();
    if (surfaceNameLower == "xplussurface")
        surfaceID = XPlusSurface;
    else if (surfaceNameLower == "xminussurface")
        surfaceID = XMinusSurface;
    else if (surfaceNameLower == "yplussurface")
        surfaceID = YPlusSurface;
    else if (surfaceNameLower == "yminussurface")
        surfaceID = YMinusSurface;
    else if (surfaceNameLower == "photodetectorscintillatorinterface")
        surfaceID = ZPlusSurface;
    else if (surfaceNameLower == "photodetectorscintillatorsurface")
        surfaceID = ZMinusSurface;
    else if (surfaceNameLower == "anysurface")
        surfaceID = AnySurface;
    else {
        surfaceID = None;
        G4cerr << "Topas is exiting due to a serious error in scoring." << G4endl;
        G4cerr << "Scorer name: " << GetName() << " has unknown surface name: " << surfaceName
               << G4endl;
        fPm->AbortSession(1);
    }
    return surfaceID;
}

G4bool FlatImager::IsOnBoundary(G4ThreeVector localpos, G4VSolid* solid, SurfaceType surfaceID) {
    G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
    // G4cout << solid->GetName() << G4endl;
    switch (surfaceID) {

    case AnySurface:
        return true;

    case XPlusSurface:
        return (std::fabs(localpos.x() - ((G4Box*) (solid))->GetXHalfLength()) < kCarTolerance);

    case XMinusSurface:
        return (std::fabs(localpos.x() + ((G4Box*) (solid))->GetXHalfLength()) < kCarTolerance);

    case YPlusSurface:
        // G4cout << "YPlus " << localpos.y() << " " << ((G4Box*) (solid))->GetYHalfLength() << G4endl;
        return (std::fabs(localpos.y() - ((G4Box*) (solid))->GetYHalfLength()) < kCarTolerance);

    case YMinusSurface:
        // G4cout << "YMinus " << localpos.y() << " " << ((G4Box*) (solid))->GetYHalfLength()
        //        << G4endl;
        return (std::fabs(localpos.y() + ((G4Box*) (solid))->GetYHalfLength()) < kCarTolerance);

    case ZPlusSurface:
        // if(std::fabs(localpos.z() - ((G4Box*) (solid))->GetZHalfLength()) < kCarTolerance)
            // G4cout << "ZPlus " << localpos.z() << " " << ((G4Box*) (solid))->GetZHalfLength() << G4endl;
        return (std::fabs(localpos.z() - ((G4Box*) (solid))->GetZHalfLength()) < kCarTolerance);

    case ZMinusSurface:
        // if(std::fabs(localpos.z() + ((G4Box*) (solid))->GetZHalfLength()) < kCarTolerance)
            // G4cout << "ZMinus " << localpos.z() << " " << ((G4Box*) (solid))->GetZHalfLength()
            //     << G4endl;
        return (std::fabs(localpos.z() + ((G4Box*) (solid))->GetZHalfLength()) < kCarTolerance);

    default:
        G4cerr << "Topas is exiting due to a serious error." << G4endl;
        G4cerr << "FlatImager::IsOnBoundary called for unknown surface of component: " << fName
               << G4endl;
        fPm->AbortSession(1);
        return false;
    }
}

G4double FlatImager::GetAreaOfSelectedSurface(
    G4VSolid* solid, SurfaceType surfaceID, G4int, G4int, G4int) {
    G4double delta_x = 2. * ((G4Box*) (solid))->GetXHalfLength();
    G4double delta_y = 2. * ((G4Box*) (solid))->GetYHalfLength();
    G4double delta_z = 2. * ((G4Box*) (solid))->GetZHalfLength();

    switch (surfaceID) {
    case XPlusSurface:
    case XMinusSurface:
        return delta_y * delta_z;

    case YPlusSurface:
    case YMinusSurface:
        return delta_x * delta_z;

    case ZPlusSurface:
    case ZMinusSurface:
        return delta_x * delta_y;

    case AnySurface:
        return (2. * (delta_y * delta_z) + (delta_x * delta_z) + (delta_x * delta_y));

    default:
        G4cerr << "Topas is exiting due to a serious error." << G4endl;
        G4cerr << "TsBox::GetAreaOfSelectedSurface called for unknown surface of component: "
               << fName << G4endl;
        fPm->AbortSession(1);
        return 0.;
    }
}

void FlatImager::NoteIndexError(
    G4double energy, G4String componentName, G4String coordinate, G4int value, G4int limit) {
#ifdef TOPAS_MT
    G4AutoLock l(&IndexErrorMutex);
#endif

    G4cerr << "\nTopas experienced an error in scoring." << G4endl;
    G4cerr << "A scorer in the Component: \"" << componentName << "\"" << G4endl;
    G4cerr << "returned " << coordinate << " index: " << value
           << " outside of the valid range of 0 to " << limit << G4endl;
    G4cerr << "We think this may sometimes happen because Geant4 navigation gets into a corrupted "
              "state."
           << G4endl;
    G4cerr << "This hit will be omitted from scoring." << G4endl;
    G4cerr << "A note at end of session will show total number of unscored steps and the total "
              "unscored energy."
           << G4endl;
    G4cerr << "In some cases the unscored energy is small enough that you can ignore this issue."
           << G4endl;
    G4cerr << "This hit's energy deposit would have been: " << energy << " MeV" << G4endl;
}

void FlatImager::NoteParameterizationError(
    G4double energy, G4String componentName, G4String volumeName) {
#ifdef TOPAS_MT
    G4AutoLock l(&ParameterizationErrorMutex);
#endif

    G4cerr << "\nTopas experienced an error in scoring." << G4endl;
    G4cerr << "A scorer in the Component: \"" << componentName << "\"" << G4endl;
    G4cerr << "has been called for a hit in the non-parameterized volume named: \"" << volumeName
           << "\"" << G4endl;
    G4cerr << "But this deos not make sense, as this scorer should only get called for hits"
           << G4endl;
    G4cerr << "in a parameterized volume (volume name would include the string \"_Division\")."
           << G4endl;
    G4cerr << "We think this may sometimes happen because Geant4 navigation gets into a corrupted "
              "state."
           << G4endl;
    G4cerr << "This hit will be omitted from scoring." << G4endl;
    G4cerr << "A note at end of session will show total number of unscored steps and the total "
              "unscored energy."
           << G4endl;
    G4cerr << "In some cases the unscored energy is small enough that you can ignore this issue."
           << G4endl;
    G4cerr << "This hit's energy deposit would have been: " << energy << " MeV" << G4endl;
}
G4int FlatImager::GetIndex(G4Step* aStep) {
    if (fPtDtDivisionCount[0] * fPtDtDivisionCount[1] * fPtDtDivisionCount[2] == 1) {
        return 0;
    } else {
        const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
        // G4cout << touchable->GetVolume()->GetName() << G4endl;
        if (IsParameterized() &&
            !aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->IsParameterised()) {
            NoteParameterizationError(aStep->GetTotalEnergyDeposit(),
                GetName(),
                aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName());
            return -1;
        }

        G4int iX = touchable->GetReplicaNumber(2);
        G4int iY = touchable->GetReplicaNumber(1);
        G4int iZ = touchable->GetReplicaNumber(0);

        if (iX < 0 || iX >= fPtDtDivisionCount[0]) {
            NoteIndexError(
                aStep->GetTotalEnergyDeposit(), GetName(), "X", iX, fPtDtDivisionCount[0] - 1);
            return -1;
        }

        if (iY < 0 || iY >= fPtDtDivisionCount[1]) {
            NoteIndexError(
                aStep->GetTotalEnergyDeposit(), GetName(), "Y", iY, fPtDtDivisionCount[1] - 1);
            return -1;
        }

        if (iZ < 0 || iZ >= fPtDtDivisionCount[2]) {
            NoteIndexError(
                aStep->GetTotalEnergyDeposit(), GetName(), "Z", iZ, fPtDtDivisionCount[2] - 1);
            return -1;
        }

        return iX * fPtDtDivisionCount[1] * fPtDtDivisionCount[2] + iY * fPtDtDivisionCount[2] + iZ;
    }
}

G4int FlatImager::GetIndex(G4int iX, G4int iY, G4int iZ) {
    if (fPtDtDivisionCount[0] * fPtDtDivisionCount[1] * fPtDtDivisionCount[2] == 1) {
        return 0;
    } else {
        return iX * fPtDtDivisionCount[1] * fPtDtDivisionCount[2] + iY * fPtDtDivisionCount[2] + iZ;
    }
}

G4int FlatImager::GetBin(G4int index, G4int iBin) {
    G4int binX = int(index / (fPtDtDivisionCount[1] * fPtDtDivisionCount[2]));
    if (iBin == 0) return binX;

    G4int binY =
        int((index - binX * fPtDtDivisionCount[1] * fPtDtDivisionCount[2]) / fPtDtDivisionCount[2]);
    if (iBin == 1) return binY;

    G4int binZ =
        index - binX * fPtDtDivisionCount[1] * fPtDtDivisionCount[2] - binY * fPtDtDivisionCount[2];
    if (iBin == 2) return binZ;

    return -1;
}

void FlatImager::GetNbOfRings(void) {
    NbOfRings = 1;
}
void FlatImager::GetNbOfDetectors(void) {
    NbOfDetectors = 1;   //fPm->GetIntegerParameter(GetFullParmName("NbOfDetectors"));
}
int FlatImager::GetCrystalBoxID(int ZBin, int XBin) {
    return ZBin * NbOfXBins + XBin;
}

void FlatImager::GetCrystalRadi(void) {
    fCrystalRadi = fPm->GetDoubleParameter(GetFullParmName("Crystal/Radius"), "Length");
    G4cout << "radius " << fCrystalRadi << G4endl;
    fCrystalRadiTotal = fCrystalRadi;
}

void FlatImager::GetCrystalReflectorThickness(void) {
    fCrystalReflectorThickness =
        fPm->ParameterExists(GetFullParmName("Crystal/ReflectorThickness"))
            ? fPm->GetDoubleParameter(GetFullParmName("Crystal/ReflectorThickness"), "Length")
            : 0.0 * mm;
    // fCrystalRadiTotal += fCrystalReflectorThickness;
    // G4cout << "Crystal radius + Reflector " << fCrystalRadiTotal << G4endl;
}

void FlatImager::GetTubeMaterial(void) {
    fTubeMaterial = "Air";   // fPm->GetStringParameter(GetFullParmName("Material"));
}

void FlatImager::GetReflectorMaterial(void) {
    fReflectorMaterial = fPm->ParameterExists(GetFullParmName("Crystal/ReflectorMaterial"))
                             ? fPm->GetStringParameter(GetFullParmName("Crystal/ReflectorMaterial"))
                             : "";
    if (fReflectorMaterial.empty()) {
        assert(fCrystalReflectorThickness == 0.0);
    }
}

void FlatImager::GetPrefilters(void) {
    fNbPrefilters = fPm->ParameterExists(GetFullParmName("Prefilters/Materials"))
                        ? fPm->GetVectorLength(GetFullParmName("Prefilters/Materials"))
                        : 0;
    if (fNbPrefilters > 0) {
        fPrefilterThicknesses =
            fPm->GetDoubleVector(GetFullParmName("Prefilters/Thicknesses"), "Length");
    }
    fTotalPrefilterThicknesses = 0;
    for (G4int i = 0; i < fNbPrefilters; i++) {
        fTotalPrefilterThicknesses += fPrefilterThicknesses[i];
    }

    if (fNbPrefilters > 0) {
        fPrefilterMaterials = fPm->GetStringVector(GetFullParmName("Prefilters/Materials"));
    }
}

void FlatImager::GetPhotoDetectorHLY(void) {

    fPhotoDetectorHLZ = fPm->GetDoubleParameter(GetFullParmName("PhotoDetector/HLZ"), "Length");
}

void FlatImager::GetPhotoDetectorMaterial(void) {

    fPhotoDetectorMaterial = fPm->GetStringParameter(GetFullParmName("PhotoDetector/Material"));
}

void FlatImager::GetPhotoDetectorXBins(void) {
    fPtDtDivisionCount[0] = fPm->ParameterExists(GetFullParmName("PhotoDetector/XBins"))
                                ? fPm->GetIntegerParameter(GetFullParmName("PhotoDetector/XBins"))
                                : 1;
}

void FlatImager::GetPhotoDetectorYBins(void) {
    fPtDtDivisionCount[1] = fPm->ParameterExists(GetFullParmName("PhotoDetector/YBins"))
                                ? fPm->GetIntegerParameter(GetFullParmName("PhotoDetector/YBins"))
                                : 1;
}

void FlatImager::GetPhotoDetectorZBins(void) {
    fPtDtDivisionCount[2] = fPm->ParameterExists(GetFullParmName("PhotoDetector/ZBins"))
                                ? fPm->GetIntegerParameter(GetFullParmName("PhotoDetector/ZBins"))
                                : 1;
}

G4double FlatImager::GetCrystalGapHLX(void) {
    if (NbOfXBins == 1) {
        if (std::abs(fHLX - fCrystalHLX) > eps) {
            G4cout << "Error: if the number of XBins is 1 then HLX should be equal "
                      "to Crystal/HLX, see: "
                   << GetFullParmName("HLX") << " and " << GetFullParmName("Crystal/HLX") << G4endl;
            exit(1);
        }
        return 0;
    }
    if ((fCrystalHLX + fCrystalReflectorThickness) * NbOfXBins > fHLX) {
        G4cout << "Error: Crystal/HLX * Crystal/NbOfXBins should be smaller than HLX, see: "
               << GetFullParmName("Crystal/NbOfXBins") << ", " << GetFullParmName("HLX") << " and "
               << GetFullParmName("Crystal/HLX") << G4endl;
        exit(1);
    }
    return (fHLX - NbOfXBins * (fCrystalHLX + fCrystalReflectorThickness)) / (NbOfXBins - 1);
}

G4double FlatImager::GetCrystalGapHLY(void) {
    if (NbOfYBins == 1) {
        if (std::abs(fHLY - fCrystalHLY) > eps) {
            G4cout << "Error: if the number of YBins is 1 then HLY should be equal "
                      "to Crystal/HLY, see: "
                   << GetFullParmName("HLY") << " and " << GetFullParmName("Crystal/HLY") << G4endl;
            exit(1);
        }
        return 0;
    }
    if ((fCrystalHLY + fCrystalReflectorThickness) * NbOfYBins > fHLY) {
        G4cout << "fHLY: " << fHLY
               << " size: " << (fCrystalHLY + fCrystalReflectorThickness) * NbOfYBins << G4endl;
        G4cout << "Error: Crystal/HLY * Crystal/NbOfYBins should be smaller than HLY, see: "
               << GetFullParmName("Crystal/NbOfYBins") << ", " << GetFullParmName("HLY") << " and "
               << GetFullParmName("Crystal/HLY") << G4endl;
        exit(1);
    }
    return (fHLY - NbOfYBins * (fCrystalHLY + fCrystalReflectorThickness)) / (NbOfYBins - 1);
}
G4double FlatImager::GetCrystalGapHLZ(void) {
    if (NbOfZBins == 1) {
        if (std::abs(fHLZ - fCrystalHLZ) > eps) {
            G4cout << "Error: if the number of ZBins is 1 then HLZ should be equal "
                      "to Crystal/HLZ, see: "
                   << GetFullParmName("HLZ") << " and " << GetFullParmName("Crystal/HLZ") << G4endl;
            exit(1);
        }
        return 0;
    }
    if ((fCrystalHLZ + fCrystalReflectorThickness) * NbOfZBins > fHLZ) {
        G4cout << "Error: Crystal/HLZ * Crystal/NbOfZBins should be smaller than HLZ, see: "
               << GetFullParmName("Crystal/NbOfZBins") << ", " << GetFullParmName("HLZ") << " and "
               << GetFullParmName("Crystal/HLZ") << G4endl;
        exit(1);
    }
    return (fHLZ - NbOfZBins * (fCrystalHLZ + fCrystalReflectorThickness)) / (NbOfZBins - 1);
}

void FlatImager::ConstructPrefilters(
    G4VPhysicalVolume* fEnvelopePhys, G4RotationMatrix* RotMatrix) {
    G4VSolid**        PrefilterSolid       = new G4VSolid*[fNbPrefilters];
    G4LogicalVolume** PrefilterLV          = new G4LogicalVolume*[fNbPrefilters];
    G4ThreeVector*    TransVectorPrefilter = new G4ThreeVector(
        0, 0, fTotalPrefilterThicknesses * 0.5 + fCrystalHLZ + fPhotoDetectorHLZ - fCollimatorHLY);
    G4cout << "prefilter: " << fTotalPrefilterThicknesses * 0.5 << " Crystal: " << fCrystalHLZ
           << " Pd: " << fPhotoDetectorHLZ << G4endl;
    G4double previous_hl = 0;

    G4String SolidName = "PrefilterSd_";
    G4String LVName    = "PrefilterLV_";
    G4String PVName    = "PrefilterPV_";
    G4cout << TransVectorPrefilter->x() << " " << TransVectorPrefilter->y() << " "
           << TransVectorPrefilter->z() << " " << fTotalPrefilterThicknesses << G4endl;
    if (fNbPrefilters > 0) {
        for (G4int prefilter_ind = 0; prefilter_ind < fNbPrefilters; prefilter_ind++) {
            G4cout << TransVectorPrefilter->z() << " " << previous_hl << " "
                   << fPrefilterThicknesses[prefilter_ind] * 0.5 << G4endl;
            TransVectorPrefilter->setZ(TransVectorPrefilter->z() - previous_hl -
                                       fPrefilterThicknesses[prefilter_ind] * 0.5);

            PVName      = "PrefilterPV_" + std::to_string(prefilter_ind);
            LVName      = "PrefilterLV_" + std::to_string(prefilter_ind);
            SolidName   = "PrefilterSd_" + std::to_string(prefilter_ind);
            previous_hl = fPrefilterThicknesses[prefilter_ind] * 0.5;
            PrefilterSolid[prefilter_ind] =
                new G4Box(SolidName, fHLX, fHLY, fPrefilterThicknesses[prefilter_ind] * 0.5);
            PrefilterLV[prefilter_ind] = CreateLogicalVolume(
                LVName, fPrefilterMaterials[prefilter_ind], PrefilterSolid[prefilter_ind]);
            ColorLV(1, 0, prefilter_ind * 200.0 / 256.0, PrefilterLV[prefilter_ind]);
            G4cout << TransVectorPrefilter->x() << " " << TransVectorPrefilter->y() << " "
                   << TransVectorPrefilter->z() << G4endl;

            auto* PFPV = CreatePhysicalVolume(
                PVName, PrefilterLV[prefilter_ind], RotMatrix, TransVectorPrefilter, fEnvelopePhys);
            G4cout << PFPV->GetTranslation().x() << " " << PFPV->GetTranslation().y() << " "
                   << PFPV->GetTranslation().z() << G4endl;
        }
    }
}

void FlatImager::ConstructPhotodetector(
    G4VPhysicalVolume* fEnvelopePhys, G4RotationMatrix* RotMatrix) {

    G4VSolid*        PhotoDetectorSolid = new G4Box("PtDtSd", fHLX, fHLY, fPhotoDetectorHLZ);
    G4LogicalVolume* PhotoDetectorLV =
        CreateLogicalVolume("PtDtLV", fPhotoDetectorMaterial, PhotoDetectorSolid);
    G4cout << "photo detector rmaterial " << fPhotoDetectorMaterial << G4endl;
    G4Material* mat = PhotoDetectorLV->GetMaterial();
    G4cout << "photo detector material " << mat->GetName() << " " << mat->GetDensity() / (g / cm3)
           << G4endl;

    G4ThreeVector* TransVectorPhotoDetector = new G4ThreeVector(
        0, 0, 0 - fCollimatorHLY - fTotalPrefilterThicknesses * 0.5 - fCrystalHLZ);

    // SetLogicalVolumeToBeSensitive(PhotoDetectorLV);
    ColorLV(G4Colour::Green(), PhotoDetectorLV);
    auto* PDPV = CreatePhysicalVolume(
        "PhDtPV", PhotoDetectorLV, RotMatrix, TransVectorPhotoDetector, fEnvelopePhys);

    G4String  divisionName;
    G4double* PtDtWidths = new G4double[3];
    PtDtWidths[0]        = 2. * fHLX;
    PtDtWidths[1]        = 2. * fHLY;
    PtDtWidths[2]        = 2. * fPhotoDetectorHLZ;
    G4double deltaX      = PtDtWidths[0] / fPtDtDivisionCount[0] / 2.;
    G4double deltaY      = PtDtWidths[1] / fPtDtDivisionCount[1] / 2.;
    G4double deltaZ      = PtDtWidths[2] / fPtDtDivisionCount[2] / 2.;

    // G4cout << PtDtWidths[0] << " " << PtDtWidths[1] << " " << PtDtWidths[2] << " " << G4endl;
    // G4cout << fPtDtDivisionCount[0] << " " << fPtDtDivisionCount[1] << " " << fPtDtDivisionCount[2]
    //        << " " << G4endl;
    // G4cout << deltaX << " " << deltaY << " " << deltaZ << " " << G4endl;
    divisionName = "PtDt_X_Division";
    G4VSolid* XDivisionSolid =
        new G4Box(divisionName, deltaX, PtDtWidths[1] / 2., PtDtWidths[2] / 2.);
    G4LogicalVolume* XDivisionLog =
        CreateLogicalVolume(divisionName, fPhotoDetectorMaterial, XDivisionSolid);
    G4VPhysicalVolume* XDivisionPhys = CreatePhysicalVolume(
        divisionName, XDivisionLog, PDPV, kXAxis, fPtDtDivisionCount[0], deltaX * 2.);
    XDivisionLog->SetVisAttributes(fPm->GetInvisible());

    // Use Replica for Y division
    divisionName                    = "PtDt_Y_Division";
    G4VSolid*        YDivisionSolid = new G4Box(divisionName, deltaX, deltaY, PtDtWidths[2] / 2.);
    G4LogicalVolume* YDivisionLog =
        CreateLogicalVolume(divisionName, fPhotoDetectorMaterial, YDivisionSolid);
    G4VPhysicalVolume* YDivisionPhys = CreatePhysicalVolume(
        divisionName, YDivisionLog, XDivisionPhys, kYAxis, fPtDtDivisionCount[1], deltaY * 2.);
    YDivisionLog->SetVisAttributes(fPm->GetInvisible());

    // Use either Replica or Parameterization for Z division
    divisionName = "PtDt_Z_Division";

    // If necessary, propagate MaxStepSize to divisions
    G4String ParmNameForDivisionMaxStepSize = "Ge/" + fName + "/" + divisionName + "/MaxStepSize";
    if ((!fPm->ParameterExists(ParmNameForDivisionMaxStepSize)) &&
        fPm->ParameterExists(GetFullParmName("MaxStepSize")))
        fPm->AddParameter("d:" + ParmNameForDivisionMaxStepSize,
            GetFullParmName("MaxStepSize") + +" " +
                fPm->GetUnitOfParameter(GetFullParmName("MaxStepSize")));

    G4VSolid*        ZDivisionSolid = new G4Box(divisionName, deltaX, deltaY, deltaZ);
    G4LogicalVolume* ZDivisionLog =
        CreateLogicalVolume(divisionName, fPhotoDetectorMaterial, ZDivisionSolid);

    // Since parameterization is not reliable in parallel worlds, only parameterize if really necessary.
    // if (fConstructParameterized)
    // 	CreatePhysicalVolume(divisionName, ZDivisionLog, YDivisionPhys, kZAxis, fDivisionCounts[2], new TsParameterisation(this));
    // else
    // 	CreatePhysicalVolume(divisionName,ZDivisionLog,YDivisionPhys,kZAxis,fDivisionCounts[2],deltaZ*2.);
    CreatePhysicalVolume(
        divisionName, ZDivisionLog, YDivisionPhys, kZAxis, fPtDtDivisionCount[2], deltaZ * 2.);
    ZDivisionLog->SetVisAttributes(GetVisAttributes(""));
    SetLogicalVolumeToBeSensitive(ZDivisionLog);
}

G4VPhysicalVolume* FlatImager::Construct(void) {
    Imager::Construct();
    // G4cerr << "Starting constructing flat-panel detector" << fName << G4endl;
    Imager::GetHLX();
    Imager::GetHLY();
    Imager::GetHLZ();
    IsCollimatorExists();
    GetNbOfRings();
    GetTubeMaterial();

    GetPhotoDetectorXBins();
    GetPhotoDetectorYBins();
    GetPhotoDetectorZBins();
    GetCrystalReflectorThickness();
    GetReflectorMaterial();

    GetNbOfDetectors();
    GetPrefilters();

    GetPhotoDetectorHLY();
    GetPhotoDetectorMaterial();

    dbg(NbOfRings);
    dbg(NbOfDetectors);
    dbg(NbOfXBins);
    dbg(NbOfYBins);
    const G4int        detectorIndex           = 0;
    G4VSolid*          CollimatorDetectorSolid = nullptr;
    G4VSolid*          CollimatorOpeningSolid  = nullptr;
    G4LogicalVolume*   CollimatorDetectorLV    = nullptr;
    G4LogicalVolume*   CollimatorOpeningLV     = nullptr;
    G4VPhysicalVolume* CollimatorDetectorPV    = nullptr;

    G4VSolid* CrystalDetectorSolid;
    G4VSolid* CrystalSolid;
    G4VSolid* ReflectorSolid;
    if (fCrystalShape == "Box") {
        CrystalDetectorSolid = new G4Box("CrDtSd", fHLX, fHLY, fCrystalHLZ);
        CrystalSolid         = new G4Box("CrSd", fCrystalHLX, fCrystalHLY, fCrystalHLZ);
        if (fCrystalReflectorThickness > 0) {
            ReflectorSolid = new G4Box("RfSd",
                fCrystalHLX + fCrystalReflectorThickness,
                fCrystalHLY + fCrystalReflectorThickness,
                fCrystalHLZ);
        }
    } else if (fCrystalShape == "Cylinder") {
        CrystalDetectorSolid = new G4Box("CrDtSd", fHLX, fHLY, fCrystalHL);
        fCrystalRadiTotal    = fCrystalRadius;
        fCrystalHLX          = fCrystalRadius;
        fCrystalHLY          = fCrystalRadius;
        fCrystalHLZ          = fCrystalHL;
        CrystalSolid         = new G4Tubs("CrSd", 0, fCrystalRadius, fCrystalHL, 0, 2 * M_PI);
        if (fCrystalReflectorThickness > 0) {
            // fCrystalHLZ += fCrystalReflectorThickness;
            // fCrystalHLX += fCrystalReflectorThickness;
            fCrystalRadiTotal += fCrystalReflectorThickness;
            ReflectorSolid =
                new G4Tubs("RfSd", fCrystalRadius, fCrystalRadiTotal, fCrystalHL, 0, 2 * M_PI);
        }
    }
    G4VSolid* EmptyBoxSolid = new G4Box("BxSd",
        fHLX,
        fHLY,
        fPhotoDetectorHLZ + fCrystalHLZ + fTotalPrefilterThicknesses * 0.5 + fCollimatorHLY);

    // G4VSolid *CrystalTubSolid = new G4Tubs("CrTbSd", 0, fCrystalRadi - fCrystalRadi * 0.05, fCrystalHLY, 0, 2 * M_PI);

    G4LogicalVolume* CrystalDetectorLV =
        CreateLogicalVolume("CrDtLV", fMaterial, CrystalDetectorSolid);
    G4LogicalVolume* ReflectorLV;
    if (fCrystalReflectorThickness > 0) {
        ReflectorLV = CreateLogicalVolume("RfTbLV", fReflectorMaterial, ReflectorSolid);
    }
    G4LogicalVolume* CrystalLV = CreateLogicalVolume("CrLV", fCrystalMaterial, CrystalSolid);
    fEnvelopeLog               = CreateLogicalVolume("BxLV", fTubeMaterial, EmptyBoxSolid);

    ColorLV(1, 1, 1, CrystalDetectorLV);
    ColorLV(0, 0, 1, fEnvelopeLog);
    // ColorLV(G4Colour::Cyan(), CrystalTubLV);
    ColorLV(1.0, 0.7529411, 0.796078, CrystalLV);

    if (fCrystalReflectorThickness > 0) {
        ColorLV(1.0, 0.29411, 0.96078, ReflectorLV);
    }

    fEnvelopePhys                   = CreatePhysicalVolume(fEnvelopeLog);
    const double      Angle         = 0;
    const G4double    RotationAngle = 0.5 * M_PI - Angle;
    G4RotationMatrix* RotMatrix     = new G4RotationMatrix;
    // RotMatrix->rotateX(0.5 * M_PI);
    // RotMatrix->rotateY(0.5 * M_PI * 2);
    RotMatrix->rotateX(0);
    RotMatrix->rotateY(0);
    // RotMatrix->rotateY(0);
    RotMatrix->rotateZ(0);

    ConstructPrefilters(fEnvelopePhys, RotMatrix);
    ConstructPhotodetector(fEnvelopePhys, RotMatrix);

    // CrystalCounter* crystalCounter = GetCrystalCounter();
    // crystalCounter->SetCurrentRing(0);
    // crystalCounter->SetCurrentDetector(detectorIndex);

    const G4double CollimatorTransX = std::sin(Angle) * (0 + fCollimatorHLY);
    const G4double CollimatorTransY = 0;   //std::sin(Angle) * (0 + fCollimatorHLY);
    const G4double CollimatorTransZ =
        std::cos(Angle) * (fTotalPrefilterThicknesses * 0.5 + fCrystalHLY + fPhotoDetectorHLZ);

    const G4double CrystalTransX = std::sin(Angle) * (0 + 2 * fCollimatorHLY);
    const G4double CrystalTransY = 0;   // std::sin(Angle) * (0 + 2 * fCollimatorHLY);
    const G4double CrystalTransZ =
        std::cos(Angle) * (fPhotoDetectorHLZ - fCollimatorHLY - fTotalPrefilterThicknesses * 0.5);
    G4ThreeVector* TransVectorCollimator =
        new G4ThreeVector(CollimatorTransX, CollimatorTransY, CollimatorTransZ);
    G4ThreeVector* TransVectorCrystal =
        new G4ThreeVector(CrystalTransX, CrystalTransY, CrystalTransZ);
    // G4ThreeVector* TransVectorCrystal2 = new G4ThreeVector(CrystalTransX, CrystalTransY, TransZ);

    if (fCollimatorExists) {
        CollimatorDetectorSolid = new G4Box("CoDtSd", fHLX, fCollimatorHLY, fHLZ);
        CollimatorOpeningSolid  = new G4Box(
            "CoOpSd", GetCollimatorOpeningHLX(), fCollimatorHLY, GetCollimatorOpeningHLZ());
        CollimatorDetectorLV =
            CreateLogicalVolume("CoDtLV", fCollimatorMaterial, CollimatorDetectorSolid);
        CollimatorOpeningLV =
            CreateLogicalVolume("CoOpLV", fCollimatorOpeningMaterial, CollimatorOpeningSolid);
        CollimatorDetectorPV = CreatePhysicalVolume("CoBxPV",
            detectorIndex,
            false,
            CollimatorDetectorLV,
            RotMatrix,
            TransVectorCollimator,
            fEnvelopePhys);
        ColorLV(G4Colour::Yellow(), CollimatorDetectorLV);
        ColorLV(1.0, 0.7529411, 0.396078, CollimatorOpeningLV);
    }

    // CrystalDetectorSolid = new G4Box("CrDtSd", fHLX, fCrystalHLY, fHLZ);

    // CrystalDetectorLV                    = CreateLogicalVolume(CrystalDetectorSolid);
    G4VPhysicalVolume* CrystalDetectorPV = CreatePhysicalVolume("CrDtPV",
        detectorIndex,
        false,
        CrystalDetectorLV,
        RotMatrix,
        TransVectorCrystal,
        fEnvelopePhys);

    G4RotationMatrix* RotMatrix2 = new G4RotationMatrix;
    RotMatrix2->rotateX(0);
    RotMatrix2->rotateX(0);
    RotMatrix2->rotateZ(0);

    G4OpticalSurface* OpSkinSurface2 = new G4OpticalSurface("SkinSurface_detect");
    OpSkinSurface2->SetModel(unified);
    OpSkinSurface2->SetType(dielectric_dielectric);
    OpSkinSurface2->SetFinish(polished);

    G4LogicalSkinSurface* CrSurf = new G4LogicalSkinSurface("CrSurf", CrystalLV, OpSkinSurface2);
    G4LogicalSkinSurface* RfSurf;
    if (fCrystalReflectorThickness > 0) {
        G4LogicalSkinSurface* RfSurf =
            new G4LogicalSkinSurface("RfSurf", ReflectorLV, OpSkinSurface2);
    }

    G4LogicalSkinSurface* ScinSurf =
        new G4LogicalSkinSurface("ScinSurf", CrystalDetectorLV, OpSkinSurface2);
    for (G4int i = 0; i < NbOfYBins; ++i) {
        const G4double YCenter = -fHLY + (2 * i + 1) * (fCrystalHLY + fCrystalReflectorThickness) +
                                 GetCrystalGapHLY() * 2 * i;
        for (G4int j = 0; j < NbOfXBins; ++j) {
            const G4double XCenter = -fHLX +
                                     (2 * j + 1) * (fCrystalHLX + fCrystalReflectorThickness) +
                                     GetCrystalGapHLX() * 2 * j;
            G4ThreeVector* TransVector = new G4ThreeVector(XCenter, YCenter, 0);
            if (fCollimatorExists) {
                CreatePhysicalVolume("CoOpBxPV",
                    GetCrystalBoxID(i, j),
                    true,
                    CollimatorOpeningLV,
                    0,
                    TransVector,
                    CollimatorDetectorPV);
            }
            // G4cerr << "Current crystal box ID: "
            //        << crystalCounter->GetCrystalBoxID(i, j) << G4endl;
            // G4cout << "Current crystal box pos: ( " << TransVector[0] << ", " << TransVector[1] << ", " << TransVector[2] << " )" << G4endl;
            if (fCrystalReflectorThickness > 0) {
                auto* rftb = CreatePhysicalVolume("RfPV",
                    i * NbOfYBins + j,
                    true,
                    ReflectorLV,
                    RotMatrix2,
                    TransVector,
                    CrystalDetectorPV);
            }
            auto* crtb = CreatePhysicalVolume("CrPV",
                i * NbOfYBins + j,
                true,
                CrystalLV,
                RotMatrix2,
                TransVector,
                CrystalDetectorPV);
        }
    }

    // delete crystalCounter;
    SetTooComplexForOGLS();
    return fEnvelopePhys;
}
