// Component for RingImager

// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "RingImager.hh"

#include "TsParameterManager.hh"

#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

#include <G4AffineTransform.hh>

const G4double eps = 1e-9;

#define dbg(x) G4cerr << #x << " = " << x << G4endl

RingImager::RingImager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
    TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :
    Imager(pM, eM, mM, gM, parentComponent, parentVolume, name) {
    fDetectorType = "PET";
}

RingImager::~RingImager() {
    ;
}

void RingImager::SetCurrentDetector(int detectorID) {
    fRingSectorID = detectorID;

    // only one module and one submodule
    fAxialModuleID         = 0;
    fTransaxialModuleID    = 0;
    fAxialSubmoduleID      = 0;
    fTransaxialSubmoduleID = 0;
    fBlockID               = fRingSectorID + fAxialRingSectorID * NbOfDetectors;
}

void RingImager::SetModuleID(int moduleIDx, int moduleIDy, int moduleIDz) {
    fTransaxialModuleID = moduleIDx;
    fLayerModuleID      = moduleIDy;
    fAxialModuleID      = moduleIDz;
}
int RingImager::GetModuleID(int moduleIDx, int moduleIDy, int moduleIDz) {
    SetModuleID(moduleIDx, moduleIDy, moduleIDz);
    int moduleID = fTransaxialModuleID;
    moduleID += fRingSectorID * NbOfTransaxialModules;
    moduleID += fAxialModuleID * NbOfModulesPerRing;
    moduleID += fLayerModuleID * NbOfModulesPerLayer;
    return moduleID;
}

void RingImager::SetSubmoduleID(int submoduleIDx, int submoduleIDy, int submoduleIDz) {
    fTransaxialSubmoduleID = submoduleIDx;
    fLayerSubmoduleID      = submoduleIDy;
    fAxialSubmoduleID      = submoduleIDz;
}

int RingImager::GetSubmoduleID(int submoduleIDx, int submoduleIDy, int submoduleIDz) {
    SetSubmoduleID(submoduleIDx, submoduleIDy, submoduleIDz);
    int submoduleID = fTransaxialSubmoduleID;
    submoduleID += fTransaxialModuleID * NbOfTransaxialSubmodules;
    submoduleID += fRingSectorID * NbOfTransaxialSubmodulesPerRSector;
    submoduleID += fAxialModuleID * NbOfSubmodulesPerAxialModule;
    submoduleID += fLayerModuleID * NbOfSubmodulesPerLayerModule;
    submoduleID += fAxialSubmoduleID * NbOfSubmodulesPerRing;
    submoduleID += fLayerSubmoduleID * NbOfSubmodulesPerLayer;
    return submoduleID;
}

void RingImager::SetCrystalID(int crystalIDx, int crystalIDy, int crystalIDz) {
    fTransaxialCrystalID = crystalIDx;
    fLayerCrystalID      = crystalIDy;
    fAxialCrystalID      = crystalIDz;
}
int RingImager::GetCrystalBoxID(int crystalIDx, int crystalIDy, int crystalIDz) {
    SetCrystalID(crystalIDx, crystalIDy, crystalIDz);
    int crystalID = fTransaxialCrystalID;
    crystalID += fTransaxialSubmoduleID * NbOfTransaxialCrystals;
    crystalID += fTransaxialModuleID * NbOfTransaxialCrystalPerModule;
    crystalID += fRingSectorID * NbOfTransaxialCrystalPerRSector;
    crystalID += fAxialSubmoduleID * NbOfTransaxialCrystalPerAxialSubmodule;
    crystalID += fLayerSubmoduleID * NbOfTransaxialCrystalPerLayerSubmodule;
    crystalID += fAxialModuleID * NbOfTransaxialCrystalPerAxialModule;
    crystalID += fLayerModuleID * NbOfTransaxialCrystalPerLayerModule;
    crystalID += fAxialCrystalID * NbOfCrystalsPerRing;
    crystalID += fLayerCrystalID * NbOfCrystalsPerLayer;
    return crystalID;
}

void RingImager::GetNbOfModulesX(void) {
    NbOfModulesX = fPm->ParameterExists(GetFullParmName("Modules/NbOfXBins"))
                       ? fPm->GetIntegerParameter(GetFullParmName("Modules/NbOfXBins"))
                       : 1;
}
void RingImager::GetNbOfModulesY(void) {
    NbOfModulesY = fPm->ParameterExists(GetFullParmName("Modules/NbOfYBins"))
                       ? fPm->GetIntegerParameter(GetFullParmName("Modules/NbOfYBins"))
                       : 1;
    if (NbOfModulesY > 1) {
        G4cerr << "Error: We only support Modules/NbOfYBins == 1, see: " << GetFullParmName("Modules/NbOfYBins")
               << G4endl;
        exit(-1);
    }
}
void RingImager::GetNbOfModulesZ(void) {
    NbOfModulesZ = fPm->ParameterExists(GetFullParmName("Modules/NbOfZBins"))
                       ? fPm->GetIntegerParameter(GetFullParmName("Modules/NbOfZBins"))
                       : 1;
}

void RingImager::GetNbOfSubmodulesX(void) {
    NbOfSubmodulesX = fPm->ParameterExists(GetFullParmName("Submodules/NbOfXBins"))
                          ? fPm->GetIntegerParameter(GetFullParmName("Submodules/NbOfXBins"))
                          : 1;
}
void RingImager::GetNbOfSubmodulesY(void) {
    NbOfSubmodulesY = fPm->ParameterExists(GetFullParmName("Submodules/NbOfYBins"))
                          ? fPm->GetIntegerParameter(GetFullParmName("Submodules/NbOfYBins"))
                          : 1;
    if (NbOfSubmodulesY > 1) {
        G4cerr << "Error: We only support Submodules/NbOfYBins == 1, see: " << GetFullParmName("Submodules/NbOfYBins")
               << G4endl;
        exit(-1);
    }
}
void RingImager::GetNbOfSubmodulesZ(void) {
    NbOfSubmodulesZ = fPm->ParameterExists(GetFullParmName("Submodules/NbOfZBins"))
                          ? fPm->GetIntegerParameter(GetFullParmName("Submodules/NbOfZBins"))
                          : 1;
}

void RingImager::GetNbOfDetectors(void) {
    NbOfDetectors = fPm->GetIntegerParameter(GetFullParmName("NbOfDetectors"));

    if (NbOfDetectors < 0) {
        NbOfDetectors = floor(M_PI / asin(fHLX / fRingRadius));
        G4cout << "Infered that NbOfDetectors = " << NbOfDetectors << G4endl;
    }

    if (NbOfDetectors == 0) {
        G4cout << "Error: NbOfDetectors should not be equal to 0 either "
                  "implicitly or explicitely, see: "
               << GetFullParmName("NbOfDetectors") << G4endl;
        exit(1);
    }
}

void RingImager::GetModuleGapHLX(void) {
    fModuleGapX = fPm->ParameterExists(GetFullParmName("Modules/XGap"))
                      ? fPm->GetDoubleParameter(GetFullParmName("Modules/XGap"), "Length")
                      : 0.0;
}

void RingImager::GetModuleGapHLY(void) {
    fModuleGapY = fPm->ParameterExists(GetFullParmName("Modules/YGap"))
                      ? fPm->GetDoubleParameter(GetFullParmName("Modules/YGap"), "Length")
                      : 0.0;
}
void RingImager::GetModuleGapHLZ(void) {
    fModuleGapZ = fPm->ParameterExists(GetFullParmName("Modules/ZGap"))
                      ? fPm->GetDoubleParameter(GetFullParmName("Modules/ZGap"), "Length")
                      : 0.0;
}

void RingImager::GetSubmoduleGapHLX(void) {
    fSubmoduleGapX = fPm->ParameterExists(GetFullParmName("Submodules/XGap"))
                         ? fPm->GetDoubleParameter(GetFullParmName("Submodules/XGap"), "Length")
                         : 0.0;
}

void RingImager::GetSubmoduleGapHLY(void) {
    fSubmoduleGapY = fPm->ParameterExists(GetFullParmName("Submodules/YGap"))
                         ? fPm->GetDoubleParameter(GetFullParmName("Submodules/YGap"), "Length")
                         : 0.0;
}
void RingImager::GetSubmoduleGapHLZ(void) {
    fSubmoduleGapZ = fPm->ParameterExists(GetFullParmName("Submodules/ZGap"))
                         ? fPm->GetDoubleParameter(GetFullParmName("Submodules/ZGap"), "Length")
                         : 0.0;
}

void RingImager::GetCrystalGapHLX(void) {
    fCrystalGapX = fPm->ParameterExists(GetFullParmName("Crystal/XGap"))
                       ? fPm->GetDoubleParameter(GetFullParmName("Crystal/XGap"), "Length")
                       : 0.0;
}

void RingImager::GetCrystalGapHLY(void) {
    fCrystalGapY = fPm->ParameterExists(GetFullParmName("Crystal/YGap"))
                       ? fPm->GetDoubleParameter(GetFullParmName("Crystal/YGap"), "Length")
                       : 0.0;
}
void RingImager::GetCrystalGapHLZ(void) {
    fCrystalGapZ = fPm->ParameterExists(GetFullParmName("Crystal/ZGap"))
                       ? fPm->GetDoubleParameter(GetFullParmName("Crystal/ZGap"), "Length")
                       : 0.0;
}

void RingImager::GetMarginRings(void) {
    fMarginRings = fPm->ParameterExists(GetFullParmName("MarginRings"))
                       ? fPm->GetDoubleParameter(GetFullParmName("MarginRings"), "Length")
                       : 0.0;
}

void RingImager::GetCrystalCoordinateSave(void) {
    fSaveCrystalCoordinate = fPm->GetBooleanParameter(GetFullParmName("SaveCrystalCoordinate"));
    if (fSaveCrystalCoordinate) {
        fCrystalCoordinateFilename = fPm->GetStringParameter(GetFullParmName("CrystalCoordinateFilename"));
    } else {
        G4cout << "SaveCrystalCoordinate not set" << G4endl;
        fCrystalCoordinateFilename = "";
    }
}

void RingImager::GetRingRadius(void) {
    fRingRadius = fPm->GetDoubleParameter(GetFullParmName("RingRadius"), "Length");
}

void RingImager::GetHLX(void) {
    if (fPm->ParameterExists(GetFullParmName("HLX"))) {
        G4cout << "HLX doesn't change the size of detector. Detector size is inferred from Module sizes" << G4endl;
    }
    fHLX = NbOfModulesX * fModuleHLX + fModuleGapX * 0.5 * (NbOfModulesX - 1);
}

void RingImager::GetHLY(void) {
    if (fPm->ParameterExists(GetFullParmName("HLY"))) {
        G4cout << "HLY doesn't change the size of detector. Detector size is inferred from Module sizes" << G4endl;
    }
    fHLY = NbOfModulesY * fModuleHLY + fModuleGapY * 0.5 * (NbOfModulesY - 1);
}
void RingImager::GetHLZ(void) {
    if (fPm->ParameterExists(GetFullParmName("HLZ"))) {
        G4cout << "HLZ doesn't change the size of detector. Detector size is inferred from Module sizes" << G4endl;
    }
    fHLZ = NbOfModulesZ * fModuleHLZ + fModuleGapZ * 0.5 * (NbOfModulesZ - 1);
}

void RingImager::GetModuleHLX(void) {
    if (fPm->ParameterExists(GetFullParmName("Modules/HLX"))) {
        G4cout << "Module/HLX doesn't change the size of module. Module size is inferred from Submodule sizes"
               << G4endl;
    }
    fModuleHLX = NbOfSubmodulesX * fSubmoduleHLX + fSubmoduleGapX * 0.5 * (NbOfSubmodulesX - 1);
}
void RingImager::GetModuleHLY(void) {
    if (fPm->ParameterExists(GetFullParmName("Modules/HLY"))) {
        G4cout << "Module/HLY doesn't change the size of module. Module size is inferred from Submodule sizes"
               << G4endl;
    }
    fModuleHLY = NbOfSubmodulesY * fSubmoduleHLY + fSubmoduleGapY * 0.5 * (NbOfSubmodulesY - 1);
}
void RingImager::GetModuleHLZ(void) {
    if (fPm->ParameterExists(GetFullParmName("Modules/HLZ"))) {
        G4cout << "Module/HLZ doesn't change the size of module. Module size is inferred from Submodule sizes"
               << G4endl;
    }
    fModuleHLZ = NbOfSubmodulesZ * fSubmoduleHLZ + fSubmoduleGapZ * 0.5 * (NbOfSubmodulesZ - 1);
}

void RingImager::GetSubmoduleHLX(void) {
    if (fPm->ParameterExists(GetFullParmName("Submodules/HLZ"))) {
        G4cout << "Submodules/HLZ doesn't change the size of submodule. Submodule size is inferred from Crystal sizes"
               << G4endl;
    }
    fSubmoduleHLX = NbOfXBins * fCrystalHLX + fCrystalGapX * 0.5 * (NbOfXBins - 1);
}
void RingImager::GetSubmoduleHLY(void) {
    if (fPm->ParameterExists(GetFullParmName("Submodules/HLZ"))) {
        G4cout << "Submodules/HLZ doesn't change the size of submodule. Submodule size is inferred from Crystal sizes"
               << G4endl;
    }
    fSubmoduleHLY = NbOfYBins * fCrystalHLY + fCrystalGapY * 0.5 * (NbOfYBins - 1);
}
void RingImager::GetSubmoduleHLZ(void) {
    if (fPm->ParameterExists(GetFullParmName("Submodules/HLZ"))) {
        G4cout << "Submodules/HLZ doesn't change the size of submodule. Submodule size is inferred from Crystal sizes"
               << G4endl;
    }
    fSubmoduleHLZ = NbOfZBins * fCrystalHLZ + fCrystalGapZ * 0.5 * (NbOfZBins - 1);
}

void RingImager::GetTubeMaterial(void) {
    fTubeMaterial = fPm->GetStringParameter(GetFullParmName("Material"));
}

// The angle of the detector depends on the DetectorType
G4double RingImager::GetAngle(int detectorID) {
    return -M_PI / 2 + 2 * M_PI * detectorID / NbOfDetectors;
}

// CrystalCounter *RingImager::GetCrystalCounter(void)
void RingImager::SetCrystalCounter(void) {
    G4cout<<"RingImager::SetCrystalCounter"<<G4endl;
    NbOfAxialModules         = NbOfModulesZ;
    NbOfLayerModules         = NbOfModulesY;
    NbOfTransaxialModules    = NbOfModulesX;
    NbOfAxialSubmodules      = NbOfSubmodulesZ;
    NbOfLayerSubmodules      = NbOfSubmodulesY;
    NbOfTransaxialSubmodules = NbOfSubmodulesX;
    NbOfAxialCrystals        = NbOfZBins;
    NbOfLayerCrystals        = NbOfYBins;
    NbOfTransaxialCrystals   = NbOfXBins;
    NbOfCrystalsPerRing  = NbOfDetectors * NbOfTransaxialModules * NbOfTransaxialSubmodules * NbOfTransaxialCrystals;
    NbOfCrystalsPerLayer = NbOfCrystalsPerRing * NbOfAxialCrystals * NbOfAxialSubmodules * NbOfAxialModules;
    NbOfCrystalsPerSubmodule               = NbOfAxialCrystals * NbOfLayerCrystals * NbOfTransaxialCrystals;
    NbOfTransaxialCrystalPerAxialSubmodule = NbOfCrystalsPerRing * NbOfAxialCrystals;
    NbOfTransaxialCrystalPerLayerSubmodule = NbOfCrystalsPerLayer * NbOfLayerCrystals;
    NbOfTransaxialCrystalPerAxialModule    = NbOfCrystalsPerRing * NbOfAxialCrystals * NbOfAxialSubmodules;
    NbOfTransaxialCrystalPerLayerModule    = NbOfCrystalsPerLayer * NbOfLayerCrystals * NbOfLayerSubmodules;
    NbOfTransaxialCrystalPerRSector        = NbOfTransaxialModules * NbOfTransaxialSubmodules * NbOfTransaxialCrystals;
    NbOfTransaxialCrystalPerModule         = NbOfTransaxialSubmodules * NbOfTransaxialCrystals;
    NbOfModulesPerDetector                 = NbOfAxialModules * NbOfLayerModules * NbOfTransaxialModules;
    NbOfModulesPerRing                     = NbOfDetectors * NbOfTransaxialModules;
    NbOfModulesPerLayer                    = NbOfModulesPerRing * NbOfAxialModules;
    NbOfSubmodulesPerModule                = NbOfAxialSubmodules * NbOfLayerSubmodules * NbOfTransaxialSubmodules;
    NbOfTransaxialSubmodulesPerRSector     = NbOfTransaxialModules * NbOfTransaxialSubmodules;
    NbOfSubmodulesPerRing                  = NbOfDetectors * NbOfTransaxialModules * NbOfTransaxialSubmodules;
    NbOfSubmodulesPerLayer                 = NbOfSubmodulesPerRing * NbOfAxialSubmodules * NbOfAxialModules;
    NbOfSubmodulesPerAxialModule           = NbOfSubmodulesPerRing * NbOfAxialSubmodules;
    NbOfSubmodulesPerLayerModule           = NbOfSubmodulesPerLayer * NbOfLayerSubmodules;
}

void RingImager::GetCastorScannerName(void) {
    fScannerName = fPm->ParameterExists(GetFullParmName("CastorName"))
                       ? fPm->GetStringParameter(GetFullParmName("CastorName"))
                       : ("topas_scanner_" + fDetectorType);
}

void RingImager::GetCastorGeomPath(void) {
    GetCastorScannerName();
    fGeomPath = (fPm->ParameterExists(GetFullParmName("CastorGeomDirectory"))
                        ? fPm->GetStringParameter(GetFullParmName("CastorGeomDirectory"))
                        : "") +
                fScannerName + ".geom";
}

void RingImager::GetCastorDescription(void) {
    fDescription = fPm->ParameterExists(GetFullParmName("CastorDescription"))
                       ? fPm->GetStringParameter(GetFullParmName("CastorDescription"))
                       : "Custom scanner geometry simulated in TOPAS";
}

void RingImager::GetCastorMinAngleDifference(void) {
    fMinAngleDifference = fPm->ParameterExists(GetFullParmName("CastorMinAngleDifference"))
                              ? fPm->GetDoubleParameter(GetFullParmName("CastorMinAngleDifference"), "Angle")
                              : 0 * deg;
}

void RingImager::GetCastorMeanDepthOfInteraction(void) {
    fMeanDepthOfInteraction = fPm->ParameterExists(GetFullParmName("CastorMeanDepthOfInteraction"))
                                  ? fPm->GetDoubleParameter(GetFullParmName("CastorMeanDepthOfInteraction"), "Length")
                                  : -mm;
}

void RingImager::GetCastorOutputSize(void) {
    fVoxelsTransaxial = fPm->ParameterExists(GetFullParmName("CastorOutputSize"))
                            ? fPm->GetIntegerParameter(GetFullParmName("CastorOutputSize"))
                            : 256;
}

void RingImager::GetCastorOutputDepth(void) {
    fVoxelsAxial = fPm->ParameterExists(GetFullParmName("CastorOutputDepth"))
                       ? fPm->GetIntegerParameter(GetFullParmName("CastorOutputDepth"))
                       : NbOfZBins * NbOfModulesZ;
}

void RingImager::GetCastorFOVSize(void) {
    fFOVTransaxial = fPm->ParameterExists(GetFullParmName("CastorFOVSize"))
                         ? fPm->GetDoubleParameter(GetFullParmName("CastorFOVSize"), "Length")
                         : 2 * (fRingRadius + 2 * fCrystalHLY);
}

void RingImager::GetCastorFOVDepth(void) {
    fFOVAxial = fPm->ParameterExists(GetFullParmName("CastorFOVDepth"))
                    ? fPm->GetDoubleParameter(GetFullParmName("CastorFOVDepth"), "Length")
                    : (NbOfModulesZ - 1) * fMarginRings + NbOfModulesZ * 2 * fHLZ;
}

G4String RingImager::WriteCopies(G4double value, G4int numCopies) {
    return WriteCopies(std::to_string(value), numCopies);
}

G4String RingImager::WriteCopies(G4int value, G4int numCopies) {
    return WriteCopies(std::to_string(value), numCopies);
}

G4String RingImager::WriteCopies(G4String value, G4int numCopies) {
    G4String output = "";
    output += value;
    for (int i = 0; i < numCopies - 1; i++) {
        output += ", " + value;
    }
    return output;
}

void RingImager::CreateCastorHeader(void) {
    static const char* header_opening = "# comments\n"
                                        "#       Y                                        _________  \n"
                                        "#       |                                       / _ \\     \\ \n"
                                        "#       |                                      | / \\ |     |\n"
                                        "#       |_____ Z                               | | | |     |\n"
                                        "#        \\                                     | | | |     |\n"
                                        "#         \\                                    | \\_/ |     |\n"
                                        "#          X                                    \\___/_____/\n"
                                        "# Left-handed axis orientation\n"
                                        "# scanner axis is z\n"
                                        "# positions in millimeters\n"
                                        "# Use comma without space as separator in the tables.\n\n";

    // This function also calls GetScannerName()
    GetCastorGeomPath();
    GetCastorOutputSize();
    GetCastorDescription();
    GetCastorOutputDepth();
    GetCastorFOVSize();
    GetCastorFOVDepth();
    G4cerr << "Writing to: " << fGeomPath << G4endl;
    std::ofstream header(fGeomPath);

    if (!header) {
        G4cout << "Could not open the header file!" << G4endl;
        exit(1);
    }
    header << header_opening << std::endl;

    // Make CASToR .geom file

    if (fDetectorType == "SPECT" || fDetectorType == "PG") {
        header << "modality: SPECT_CONVERGENT" << std::endl;
        header << "scanner name: " << fScannerName << std::endl;
        header << "description: " << fDescription << std::endl;
        header << "number of detector heads: " << 1 << std::endl;
        header << std::endl;
        header << "trans number of pixels: " << NbOfXBins << std::endl;
        header << "trans pixel size: " << 2 * fCrystalHLX / mm << std::endl;
        header << "trans gap size: " << 2 * fCrystalGapX / mm << std::endl;
        header << std::endl;
        header << "axial number of pixels: " << NbOfZBins << std::endl;
        header << "axial pixel size: " << 2 * fCrystalHLZ / mm << std::endl;
        header << "axial gap size: " << 2 * fCrystalGapZ / mm << std::endl;
        header << std::endl;
        header << "detector depth: " << 2 * fCrystalHLY / mm << std::endl;
        header << "scanner radius: " << fRingRadius / mm << std::endl;
        header << std::endl;
        header << "head1:" << std::endl;
        header << "trans focal model: constant #if parallel hole" << std::endl;
        header << "trans number of coef model: " << 1 << std::endl;
        header << "trans parameters: " << 0 << std::endl;
        header << "axial focal model: constant" << std::endl;
        header << "axial number of coef model: " << 1 << std::endl;
        header << "axial parameters: " << 0 << std::endl;
        header << std::endl;
        header << "voxels number transaxial: " << fVoxelsTransaxial << std::endl;
        header << "voxels number axial: " << fVoxelsAxial << std::endl;
        header << "field of view transaxial: " << fFOVTransaxial / mm << std::endl;
        header << "field of view axial: " << fFOVAxial / mm << std::endl;
    } else if (fDetectorType == "PET") {
        GetCastorMeanDepthOfInteraction();
        GetCastorMinAngleDifference();

        header << "modality: PET" << std::endl;
        header << "scanner name: " << fScannerName << std::endl;
        header << "number of elements: " << NbOfXBins * NbOfModulesX * NbOfZBins * NbOfModulesZ * NbOfDetectors
               << std::endl;
        header << "number of layers: " << 1 << std::endl;
        header << std::endl;
        header << "voxels number transaxial: " << fVoxelsTransaxial << std::endl;
        header << "voxels number axial: " << fVoxelsAxial << std::endl;
        header << "field of view transaxial: " << fFOVTransaxial / mm << std::endl;
        header << "field of view axial: " << fFOVAxial / mm << std::endl;
        header << std::endl;
        header << "description: " << fDescription << std::endl;
        header << "scanner radius: " << fRingRadius / mm << std::endl;
        header << "number of rsectors: " << NbOfDetectors << std::endl;
        header << "number of crystals transaxial: " << NbOfXBins << std::endl;
        header << "number of crystals axial: " << NbOfZBins << std::endl;
        header << std::endl;
        header << "crystals size depth: " << 2 * fCrystalHLY / mm << std::endl;
        header << "crystals size transaxial: " << 2 * fCrystalHLX / mm << std::endl;
        header << "crystals size axial: " << 2 * fCrystalHLZ / mm << std::endl;
        header << std::endl;
        header << "rsectors first angle: " << 0 << std::endl;
        header << "number of rsectors axial: " << 1 << std::endl;
        header << "rsector gap transaxial: " << 0 << std::endl;
        header << "rsector gap axial: " << 0 << std::endl;
        header << "number of modules transaxial: " << NbOfModulesX << std::endl;
        header << "number of modules axial: " << NbOfModulesZ << std::endl;
        header << "module gap transaxial: " << 0 << std::endl;
        header << "module gap axial: " << fMarginRings / mm << std::endl;
        header << "number of submodules transaxial: " << 1 << std::endl;
        header << "number of submodules axial: " << 1 << std::endl;
        header << "submodule gap transaxial: " << 0 << std::endl;
        header << "submodule gap axial: " << 0 << std::endl;
        header << "crystal gap transaxial: " << 2 * fCrystalGapX / mm << std::endl;
        header << "crystal gap axial: " << 2 * fCrystalGapZ / mm << std::endl;
        header << "mean depth of interaction: " << fMeanDepthOfInteraction / mm << std::endl;
        header << "rotation direction: "
               << "CW" << std::endl;
        header << "min angle difference: " << fMinAngleDifference / deg << std::endl;
    }

    header.close();
}
void RingImager::ConstructDetector(void) {
    G4double           Angle, RotationAngle, DetectorTransX, DetectorTransY;
    G4double           CollimatorTransX, CollimatorTransY;
    G4VPhysicalVolume* DetectorPV;
    for (int DetectorIndex = 0; DetectorIndex < NbOfDetectors; DetectorIndex++) {
        DetectorSolid           = new G4Box("DtSd", fHLX, fHLY, fHLZ + fMarginRings * NbOfModulesZ);
        DetectorLV              = CreateLogicalVolume("DtLV", fTubeMaterial, DetectorSolid);
        CollimatorDetectorSolid = fCollimatorExists ? new G4Box("CoDtSd", fHLX, fCollimatorHL, fHLZ) : nullptr;
        CollimatorDetectorLV =
            fCollimatorExists ? CreateLogicalVolume("CoDtLV", fCollimatorMaterial, CollimatorDetectorSolid) : nullptr;
        CollimatorDetectorPV = nullptr;
        if (fCollimatorExists) {
            ColorLV(G4Colour::Green(), CollimatorDetectorLV);
        }
        ColorLV(G4Colour::Magenta(), DetectorLV);
        // fCrystalCounter->SetDetectorID(DetectorIndex);
        SetCurrentDetector(DetectorIndex);
        Angle         = -GetAngle(DetectorIndex);
        RotationAngle = 0.5 * M_PI - Angle;
        // RotationAngle= Angle;
        G4RotationMatrix* RotMatrix = new G4RotationMatrix;
        RotMatrix->rotateX(0);
        RotMatrix->rotateY(0);
        RotMatrix->rotateZ(RotationAngle);
        DetectorTransX                     = std::cos(Angle) * (fRingRadius + 2 * fCollimatorHL + fHLY);
        DetectorTransY                     = std::sin(Angle) * (fRingRadius + 2 * fCollimatorHL + fHLY);
        G4ThreeVector* TransVectorDetector = new G4ThreeVector(DetectorTransX, DetectorTransY, 0);
        G4cout << "Detector ID " << DetectorIndex << G4endl;
        DetectorPV = CreatePhysicalVolume(
            "DtPV", DetectorIndex, true, DetectorLV, RotMatrix, TransVectorDetector, fEnvelopePhys);
        
        if (fCollimatorExists) {
            CollimatorTransX                     = std::cos(Angle) * (fRingRadius + fCollimatorHL);
            CollimatorTransY                     = std::sin(Angle) * (fRingRadius + fCollimatorHL);
            G4ThreeVector* TransVectorCollimator = new G4ThreeVector(CollimatorTransX, CollimatorTransY, 0);
            CollimatorDetectorPV                 = CreatePhysicalVolume(
                "CoBxPV", DetectorIndex, true, CollimatorDetectorLV, RotMatrix, TransVectorCollimator, fEnvelopePhys);
        }
        ConstructModules(DetectorPV, CollimatorDetectorPV);
    }
}

void RingImager::ConstructModules(G4VPhysicalVolume* DetectorPV, G4VPhysicalVolume* CollimatorPV) {
    G4int    moduleID = 0;
    G4double moduleTransX, moduleTransY, moduleTransZ;
    for (int moduleIndexY = 0; moduleIndexY < NbOfModulesY; moduleIndexY++) {
        moduleTransY = -((fModuleHLY * 2) * NbOfModulesY + fModuleGapY * (NbOfModulesY - 1)) * 0.5 + fModuleHLY +
                       (fModuleHLY * 2 + fModuleGapY) * moduleIndexY;
        assert(moduleTransY > -fHLY && moduleTransY < fHLY);

        for (int moduleIndexZ = 0; moduleIndexZ < NbOfModulesZ; moduleIndexZ++) {
            moduleTransZ = -((fModuleHLZ * 2) * NbOfModulesZ + fModuleGapZ * (NbOfModulesZ - 1)) * 0.5 + fModuleHLZ +
                           (fModuleHLZ * 2 + fModuleGapZ) * moduleIndexZ;
            assert(moduleTransZ >= -fHLZ && moduleTransZ <= fHLZ);

            for (int moduleIndexX = 0; moduleIndexX < NbOfModulesX; moduleIndexX++) {
                ModuleSolid = new G4Box("MdSd", fModuleHLX, fModuleHLY, fModuleHLZ);
                ModuleLV    = CreateLogicalVolume("MdLV", fTubeMaterial, ModuleSolid);
                ColorLV(G4Colour::Blue(), ModuleLV);
                moduleID     = GetModuleID(moduleIndexX, moduleIndexY, moduleIndexZ);
                moduleTransX = -((fModuleHLX * 2) * NbOfModulesX + fModuleGapX * (NbOfModulesX - 1)) * 0.5 +
                               fModuleHLX + (fModuleHLX * 2 + fModuleGapX) * moduleIndexX;
                assert(moduleTransX > -fHLX && moduleTransX < fHLX);
                G4ThreeVector*     TransVectorModule = new G4ThreeVector(moduleTransX, moduleTransY, moduleTransZ);
                G4VPhysicalVolume* ModulePV =
                    CreatePhysicalVolume("MdPV", moduleID, true, ModuleLV, 0, TransVectorModule, DetectorPV);
                ConstructSubmodules(ModulePV, TransVectorModule, CollimatorPV);
                assert(std::find(moduleIDs.begin(), moduleIDs.end(), moduleID) == moduleIDs.end());
                moduleIDs.push_back(moduleID);
            }
        }
    }
}
void RingImager::ConstructSubmodules(
    G4VPhysicalVolume* ModulePV, G4ThreeVector* TransVectorModule, G4VPhysicalVolume* CollimatorPV) {
    G4int    submoduleID = -1;
    G4double submoduleTransX, submoduleTransY, submoduleTransZ;
    for (int submoduleIndexY = 0; submoduleIndexY < NbOfSubmodulesY; submoduleIndexY++) {
        submoduleTransY = -((fSubmoduleHLY * 2) * NbOfSubmodulesY + fSubmoduleGapY * (NbOfSubmodulesY - 1)) * 0.5 +
                          fSubmoduleHLY + (fSubmoduleHLY * 2 + fSubmoduleGapY) * submoduleIndexY;
        assert(submoduleTransY > -fModuleHLY && submoduleTransY < fModuleHLY);
        for (int submoduleIndexZ = 0; submoduleIndexZ < NbOfSubmodulesZ; submoduleIndexZ++) {
            submoduleTransZ = -((fSubmoduleHLZ * 2) * NbOfSubmodulesZ + fSubmoduleGapZ * (NbOfSubmodulesZ - 1)) * 0.5 +
                              fSubmoduleHLZ + (fSubmoduleHLZ * 2 + fSubmoduleGapZ) * submoduleIndexZ;
            assert(submoduleTransZ > -fModuleHLZ && submoduleTransZ < fModuleHLZ);
            for (int submoduleIndexX = 0; submoduleIndexX < NbOfSubmodulesX; submoduleIndexX++) {
                SubmoduleSolid = new G4Box("SbMdSd", fSubmoduleHLX, fSubmoduleHLY, fSubmoduleHLZ);
                SubmoduleLV    = CreateLogicalVolume("SbMdLV", fTubeMaterial, SubmoduleSolid);
                ColorLV(G4Colour::Yellow(), SubmoduleLV);
                submoduleID = GetSubmoduleID(submoduleIndexX, submoduleIndexY, submoduleIndexZ);
                submoduleTransX =
                    -((fSubmoduleHLX * 2) * NbOfSubmodulesX + fSubmoduleGapX * (NbOfSubmodulesX - 1)) * 0.5 +
                    fSubmoduleHLX + (fSubmoduleHLX * 2 + fSubmoduleGapX) * submoduleIndexX;
                assert(submoduleTransX > -fModuleHLX && submoduleTransX < fModuleHLX);

                G4ThreeVector* TransVectorSubmodule =
                    new G4ThreeVector(submoduleTransX, submoduleTransY, submoduleTransZ);
                G4VPhysicalVolume* SubmodulePV =
                    CreatePhysicalVolume("SbMdPV", submoduleID, true, SubmoduleLV, 0, TransVectorSubmodule, ModulePV);
                G4ThreeVector* TransVectorCollimator = new G4ThreeVector(
                    TransVectorModule->x() + submoduleTransX, 0, TransVectorModule->z() + submoduleTransZ);
                ConstructCrystals(SubmodulePV, TransVectorCollimator, CollimatorPV);
                assert(std::find(submoduleIDs.begin(), submoduleIDs.end(), submoduleID) == submoduleIDs.end());
                submoduleIDs.push_back(submoduleID);
            }
        }
    }
}
void RingImager::ConstructCrystals(
    G4VPhysicalVolume* SubmodulePV, G4ThreeVector* TransVectorSubmodule, G4VPhysicalVolume* CollimatorPV) {
    G4int    crystalID    = -1;
    G4int    collimatorID = 0;
    G4double crystalTransX, crystalTransY, crystalTransZ;
    for (int crystalIndexY = 0; crystalIndexY < NbOfYBins; crystalIndexY++) {
        crystalTransY = -((fCrystalHLY * 2) * NbOfYBins + fCrystalGapY * (NbOfYBins - 1)) * 0.5 + fCrystalHLY +
                        (fCrystalHLY * 2 + fCrystalGapY) * crystalIndexY;
        assert(crystalTransY > -fSubmoduleHLY && crystalTransY < fSubmoduleHLY);

        for (int crystalIndexZ = 0; crystalIndexZ < NbOfZBins; crystalIndexZ++) {
            crystalTransZ = -((fCrystalHLZ * 2) * NbOfZBins + fCrystalGapZ * (NbOfZBins - 1)) * 0.5 + fCrystalHLZ +
                            (fCrystalHLZ * 2 + fCrystalGapZ) * crystalIndexZ;
            assert(crystalTransZ > -fSubmoduleHLZ && crystalTransZ < fSubmoduleHLZ);
            for (int crystalIndexX = 0; crystalIndexX < NbOfXBins; crystalIndexX++) {

                crystalID = GetCrystalBoxID(crystalIndexX, crystalIndexY, crystalIndexZ);

                crystalTransX = -((fCrystalHLX * 2) * NbOfXBins + fCrystalGapX * (NbOfXBins - 1)) * 0.5 + fCrystalHLX +
                                (fCrystalHLX * 2 + fCrystalGapX) * crystalIndexX;
                assert(crystalTransX > -fSubmoduleHLX && crystalTransX < fSubmoduleHLX);
                G4ThreeVector*     TransVectorCrystal = new G4ThreeVector(crystalTransX, crystalTransY, crystalTransZ);
                G4VPhysicalVolume* CrystalPV =
                    CreatePhysicalVolume("CrPV", crystalID, true, CrystalLV, 0, TransVectorCrystal, SubmodulePV);

                if (fCollimatorExists && crystalIndexY == 0) {
                    collimatorID                         = crystalID;
                    G4ThreeVector* TransVectorCollimator = new G4ThreeVector(
                        TransVectorSubmodule->x() + crystalTransX, 0, TransVectorSubmodule->z() + crystalTransZ);
                    G4cout<<"Crystal translation: "<<TransVectorCrystal->x()<<" "<<TransVectorCrystal->y()<<" "<<TransVectorCrystal->z()<<G4endl;
                    G4cout<<"Collimator translation: "<<TransVectorCollimator->x()<<" "<<TransVectorCollimator->y()<<" "<<TransVectorCollimator->z()<<" "<<G4endl;
                    CreatePhysicalVolume(
                        "CoOpPV", collimatorID, true, CollimatorOpeningLV, 0, TransVectorCollimator, CollimatorPV);
                    assert(std::find(collimatorIDs.begin(), collimatorIDs.end(), collimatorID) == collimatorIDs.end());
                    collimatorIDs.push_back(collimatorID);
                }
                assert(std::find(crystalIDs.begin(), crystalIDs.end(), crystalID) == crystalIDs.end());
                crystalIDs.push_back(crystalID);
            }
        }
    }
}

void RingImager::SaveCrystalCoordinate(void) {
    std::ofstream fCrystalCoordinateFile;
    fCrystalCoordinateFile.open(fCrystalCoordinateFilename + ".csv", std::ofstream::out);

    G4cout << "Nb of daughter :" << fEnvelopeLog->GetNoDaughters() << G4endl;
    for (int d0 = 0; d0 < fEnvelopeLog->GetNoDaughters(); d0++) {
        G4VPhysicalVolume* daughter1 = fEnvelopeLog->GetDaughter(d0);   // Detector
        G4cout << d0 << " : " << daughter1->GetName() << " " << daughter1->GetFrameTranslation() << G4endl;
        const G4ThreeVector     det_translation = daughter1->GetTranslation();
        const G4RotationMatrix* det_rotation    = daughter1->GetRotation();
        G4LogicalVolume*        DetectorLV1     = daughter1->GetLogicalVolume();
        G4cout << d0 << " : " << daughter1->GetName() << " " << G4endl;
        G4AffineTransform Tm_module(det_rotation, det_translation);
        for (int d1 = 0; d1 < DetectorLV1->GetNoDaughters(); d1++) {
            G4VPhysicalVolume*      daughter2          = DetectorLV1->GetDaughter(d1);
            G4LogicalVolume*        ModuleLV1          = daughter2->GetLogicalVolume();
            const G4ThreeVector     module_translation = daughter2->GetTranslation();
            const G4RotationMatrix* module_rotation    = daughter2->GetRotation();

            G4ThreeVector module_pos = Tm_module.TransformPoint(module_translation);
            fCrystalCoordinateFile << d1 << " " << daughter2->GetName() << " " << module_pos << G4endl;
            for (int d2 = 0; d2 < ModuleLV1->GetNoDaughters(); d2++) {
                G4VPhysicalVolume*      daughter3             = ModuleLV1->GetDaughter(d2);
                G4LogicalVolume*        SubModuleLV1          = daughter3->GetLogicalVolume();
                const G4ThreeVector     submodule_translation = daughter3->GetTranslation();
                const G4RotationMatrix* submodule_rotation    = daughter3->GetRotation();
                G4AffineTransform       Tm_submodule(module_rotation, module_translation);
                G4ThreeVector           submodule_pos =
                    Tm_module.TransformPoint(Tm_submodule.TransformPoint(submodule_translation));
                fCrystalCoordinateFile << d2 << " " << daughter3->GetName() << " " << submodule_pos << G4endl;
                for (int d3 = 0; d3 < SubModuleLV1->GetNoDaughters(); d3++) {
                    G4VPhysicalVolume* daughter4           = SubModuleLV1->GetDaughter(d3);
                    G4ThreeVector      crystal_translation = daughter4->GetTranslation();
                    G4AffineTransform  Tm_crystal(submodule_rotation, submodule_translation);
                    G4ThreeVector      crystal_pos = Tm_module.TransformPoint(
                        Tm_submodule.TransformPoint(Tm_crystal.TransformPoint(crystal_translation)));
                    fCrystalCoordinateFile << d3 << " " << daughter4->GetName() << " " << crystal_pos << G4endl;
                }
            }
        }
    }
    fCrystalCoordinateFile.close();
    exit(-1);
}
// Create the detector geometry in the simulation
G4VPhysicalVolume* RingImager::Construct(void) {
     BeginConstruction();
     G4cout << "Starting constructing " << fName << G4endl;
    Imager::CommonParameters();
    GetRingRadius();
    GetNbOfDetectors();
    GetMarginRings();
    GetCrystalGapHLX();
    GetCrystalGapHLY();
    GetCrystalGapHLZ();
    GetSubmoduleGapHLX();
    GetSubmoduleGapHLY();
    GetSubmoduleGapHLZ();
    GetModuleGapHLX();
    GetModuleGapHLY();
    GetModuleGapHLZ();

    GetNbOfModulesX();
    GetNbOfModulesY();
    GetNbOfModulesZ();

    GetNbOfSubmodulesX();
    GetNbOfSubmodulesY();
    GetNbOfSubmodulesZ();
    GetSubmoduleHLZ();
    GetSubmoduleHLY();
    GetSubmoduleHLX();
    GetModuleHLZ();
    GetModuleHLY();
    GetModuleHLX();
    GetHLZ();
    GetHLY();
    GetHLX();

    dbg(fRingRadius);
    dbg(NbOfModulesX);
    dbg(NbOfModulesZ);
    dbg(NbOfDetectors);
    dbg(NbOfXBins);
    dbg(NbOfZBins);
    dbg(fSubmoduleHLX);
    dbg(fSubmoduleHLX);
    dbg(fModuleHLX);
    
    GetTubeMaterial();
    GetCrystalCoordinateSave();

    double maxNbDetectors = floor(M_PI / asin(fHLX / fRingRadius));
    G4VSolid* EmptyTubeSolid = new G4Tubs("TbSd",
        fRingRadius,
        (fRingRadius + 2 * fHLY + 2 * fCollimatorHL) / cos(M_PI / maxNbDetectors),
        (fModuleHLZ + fModuleGapZ * 0.5 + fMarginRings) * NbOfModulesZ,
        0,
        2 * M_PI);
    G4cout<<"fRingRadius "<<fRingRadius<<" fHLY "<<fHLY<<" fCollimatorHL "<<fCollimatorHL<<" fModuleHLZ "<<fModuleHLZ<<G4endl;
    fEnvelopeLog = CreateLogicalVolume(EmptyTubeSolid);
    ColorLV(1, 1, 1, fEnvelopeLog);
    G4cout<<"SetCrystalCounter"<<G4endl;

    fEnvelopePhys = CreatePhysicalVolume(fEnvelopeLog);
    G4cout<<"SetCrystalCounter"<<G4endl;

    SetCrystalCounter();
    G4cout<<"SetCrystalCounter"<<G4endl;
    G4int NbOfModules    = NbOfModulesX * NbOfModulesY * NbOfModulesZ * NbOfDetectors;
    G4int NbOfSubmodules = NbOfSubmodulesX * NbOfSubmodulesY * NbOfSubmodulesZ * NbOfModules;
    G4int NbOfCrystals   = NbOfXBins * NbOfYBins * NbOfZBins * NbOfSubmodules;

    CrystalSolid = new G4Box("CrSd", fCrystalHLX, fCrystalHLY, fCrystalHLZ);
    CrystalLV    = CreateLogicalVolume("CrLV", fCrystalMaterial, CrystalSolid);
    SetLogicalVolumeToBeSensitive(CrystalLV);
    std::vector<G4LogicalVolume*> sensitive_volumes=GetLogicalVolumesToBeSensitive();
    for(int ssss=0;ssss< sensitive_volumes.size();ssss++){
        G4cout<<"sensitive volumes: "<< sensitive_volumes[ssss]->GetName()<<G4endl;

    }
    ColorLV(G4Colour::Red(), CrystalLV);
    CollimatorOpeningSolid =
        fCollimatorExists
            ? new G4Box("CoOpSd", Imager::GetCollimatorOpeningHLX(), fCollimatorHL, Imager::GetCollimatorOpeningHLZ())
            : nullptr;
    CollimatorOpeningLV =
        fCollimatorExists ? CreateLogicalVolume("CoOpLV", fCollimatorOpeningMaterial, CollimatorOpeningSolid) : nullptr;

    if (fCollimatorExists) {
//        ColorLV(1.0, 0.7529411, 0.796078, CollimatorOpeningLV);
        ColorLV(G4Colour::Blue(), CollimatorOpeningLV);
    }

    ConstructDetector();
    if (fSaveCrystalCoordinate) {
        SaveCrystalCoordinate();
    }
    int minInd1 = *std::min_element(moduleIDs.begin(), moduleIDs.end());
    int maxInd1 = *std::max_element(moduleIDs.begin(), moduleIDs.end());
    assert(minInd1 == 0 && maxInd1 == NbOfModulesX * NbOfModulesZ * NbOfModulesY * NbOfDetectors - 1);

    int minInd2 = *std::min_element(submoduleIDs.begin(), submoduleIDs.end());
    int maxInd2 = *std::max_element(submoduleIDs.begin(), submoduleIDs.end());
    assert(minInd2 == 0 && maxInd2 == NbOfModulesX * NbOfSubmodulesX * NbOfModulesZ * NbOfSubmodulesZ * NbOfModulesY *
                                              NbOfSubmodulesY * NbOfDetectors -
                                          1);

    int minInd3 = *std::min_element(crystalIDs.begin(), crystalIDs.end());
    int maxInd3 = *std::max_element(crystalIDs.begin(), crystalIDs.end());
    std::sort(crystalIDs.begin(), crystalIDs.end());   // {1 1 2 3 4 4 5}
    auto last = std::unique(crystalIDs.begin(), crystalIDs.end());
    crystalIDs.erase(last, crystalIDs.end());
    assert(minInd3 == 0 && maxInd3 == NbOfXBins * NbOfModulesX * NbOfSubmodulesX * NbOfZBins * NbOfModulesZ *
                                              NbOfSubmodulesZ * NbOfYBins * NbOfModulesY * NbOfSubmodulesY *
                                              NbOfDetectors -
                                          1);

    SetTooComplexForOGLS();
    return fEnvelopePhys;
}
