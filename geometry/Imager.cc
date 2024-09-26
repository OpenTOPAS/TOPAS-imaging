// Component for Imager

// Copyright 2021 Hoyeon Lee (hlee80@mgh.harvard.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "Imager.hh"

#include "TsParameterManager.hh"

#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

const G4double eps = 1e-9;

Imager::Imager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
    TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :
    TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name) {
    ;
}

Imager::~Imager() {
    ;
}

void Imager::ColorLV(G4Colour color, G4LogicalVolume* LV) {
    G4VisAttributes* yokeColor = new G4VisAttributes(color);
    RegisterVisAtt(yokeColor);
    LV->SetVisAttributes(yokeColor);
}

void Imager::ColorLV(G4double r, G4double g, G4double b, G4LogicalVolume* LV) {
    ColorLV(G4Colour(r, g, b), LV);
}

void Imager::IsCollimatorExists(void) {
    fCollimatorExists = fPm->ParameterExists(GetFullParmName("Collimator/Exists"))
                            ? fPm->GetBooleanParameter(GetFullParmName("Collimator/Exists")): false;    
}

void Imager::GetNbOfXBins(void) {
    NbOfXBins = fPm->GetIntegerParameter(GetFullParmName("Crystal/NbOfXBins"));
    if (NbOfXBins <= 0) {
        G4cerr << "Error: Crystal/NbOfXBins should be a positive integer, see: " << GetFullParmName("Crystal/NbOfXBins")
               << G4endl;
        exit(1);
    }
}

void Imager::GetNbOfYBins(void) {
    NbOfYBins = fPm->ParameterExists(GetFullParmName("Crystal/NbOfYBins"))
                    ? fPm->GetIntegerParameter(GetFullParmName("Crystal/NbOfYBins"))
                    : 1;

    if (NbOfYBins <= 0) {
        G4cerr << "Error: Crystal/NbOfYBins should be a positive integer, see: " << GetFullParmName("Crystal/NbOfYBins")
               << G4endl;
        exit(1);
    }
}

void Imager::GetNbOfZBins(void) {
    NbOfZBins = fPm->GetIntegerParameter(GetFullParmName("Crystal/NbOfZBins"));
    if (NbOfZBins <= 0) {
        G4cerr << "Error: Crystal/NbOfZBins should be a positive integer, see: " << GetFullParmName("Crystal/NbOfZBins")
               << G4endl;
        exit(1);
    }
}

G4double Imager::GetCollimatorOpeningHLX(void) {
    assert(fCollimatorExists);
    return (1 - fPm->IGetUnitlessParameter(GetFullParmName("Collimator/XSeptaThicknessPercentage"))) *fCrystalHLX;//* fHLX / NbOfXBins;
}

G4double Imager::GetCollimatorOpeningHLY(void) {
    assert(fCollimatorExists);
    return (1 - fPm->IGetUnitlessParameter(GetFullParmName("Collimator/YSeptaThicknessPercentage"))) *fCrystalHLY;//* fHLX / NbOfXBins;
}

G4double Imager::GetCollimatorOpeningHLZ(void) {
    assert(fCollimatorExists);
    return (1 - fPm->IGetUnitlessParameter(GetFullParmName("Collimator/ZSeptaThicknessPercentage"))) *fCrystalHLZ;//* fHLZ / NbOfZBins;
}

G4double Imager::GetCrystalGapHLX(void) {
    if (NbOfXBins == 1) {
        if (std::abs(fHLX - fCrystalHLX) > eps) {
            G4cerr << "Error: if the number of XBins is 1 then HLX should be equal "
                      "to Crystal/HLX, see: "
                   << GetFullParmName("HLX") << " and " << GetFullParmName("Crystal/HLX") << G4endl;
            exit(1);
        }
        return 0;
    }
    if (fCrystalHLX * NbOfXBins > fHLX) {
        G4cerr << "Error: Crystal/HLX * Crystal/NbOfXBins should be smaller than HLX, see: "
               << GetFullParmName("Crystal/NbOfXBins") << ", " << GetFullParmName("HLX") << " and "
               << GetFullParmName("Crystal/HLX") << G4endl;
        exit(1);
    }
    return (fHLX - NbOfXBins * fCrystalHLX) / (NbOfXBins - 1);
}

G4double Imager::GetCrystalGapHLZ(void) {
    if (NbOfZBins == 1) {
        if (std::abs(fHLZ - fCrystalHLZ) > eps) {
            G4cerr << "Error: if the number of ZBins is 1 then HLZ should be equal "
                      "to Crystal/HLZ, see: "
                   << GetFullParmName("HLZ") << " and " << GetFullParmName("Crystal/HLZ") << G4endl;
            exit(1);
        }
        return 0;
    }
    if (fCrystalHLZ * NbOfZBins > fHLZ) {
        G4cerr << "Error: Crystal/HLZ * Crystal/NbOfZBins should be smaller than HLZ, see: "
               << GetFullParmName("Crystal/NbOfZBins") << ", " << GetFullParmName("HLZ") << " and "
               << GetFullParmName("Crystal/HLZ") << G4endl;
        exit(1);
    }
    return (fHLZ - NbOfZBins * fCrystalHLZ) / (NbOfZBins - 1);
}

void Imager::GetHLX(void) {
    fHLX = fPm->GetDoubleParameter(GetFullParmName("HLX"), "Length");
}

void Imager::GetHLY(void) {
    fHLY = fPm->GetDoubleParameter(GetFullParmName("HLY"), "Length");
}

void Imager::GetHLZ(void) {
    fHLZ = fPm->GetDoubleParameter(GetFullParmName("HLZ"), "Length");
}

void Imager::GetMaterial(void) {
    fMaterial = fPm->GetStringParameter(GetFullParmName("Material"));
}

void Imager::GetCrystalShape(void) {
    fCrystalShape = fPm->ParameterExists(GetFullParmName("Crystal/Shape"))
                        ? fPm->GetStringParameter(GetFullParmName("Crystal/Shape"))
                        : "Box";
    
}
void Imager::GetCrystalRadius(void) {
    fCrystalRadius = fPm->GetDoubleParameter(GetFullParmName("Crystal/Radius"), "Length");
    fCrystalHLX=fCrystalRadius;
    fCrystalHLY=fCrystalRadius;
}
void Imager::GetCrystalHL(void) {
    fCrystalHL = fPm->GetDoubleParameter(GetFullParmName("Crystal/HL"), "Length");
    fCrystalHLZ          = fCrystalHL;

}
void Imager::GetCrystalHLX(void) {
    fCrystalHLX = fPm->GetDoubleParameter(GetFullParmName("Crystal/HLX"), "Length");
}

void Imager::GetCrystalHLY(void) {
    fCrystalHLY = fPm->GetDoubleParameter(GetFullParmName("Crystal/HLY"), "Length");
}

void Imager::GetCrystalHLZ(void) {
    fCrystalHLZ = fPm->GetDoubleParameter(GetFullParmName("Crystal/HLZ"), "Length");
}

void Imager::GetCollimatorHLY(void) {
    fCollimatorHL = fCollimatorExists ? fPm->GetDoubleParameter(GetFullParmName("Collimator/HL"), "Length") : 0;
}

void Imager::GetCollimatorOpeningMaterial(void) {
    fCollimatorOpeningMaterial = fPm->ParameterExists(GetFullParmName("Collimator/OpeningMaterial"))
                                     ? fPm->GetStringParameter(GetFullParmName("Collimator/OpeningMaterial"))
                                     : "Air";
}

void Imager::GetCollimatorMaterial(void) {
    // If collimator does not exist the we put a whatever material we want since
    // we're not going to use the collimator
    fCollimatorMaterial = fCollimatorExists ? fPm->GetStringParameter(GetFullParmName("Collimator/Material")) : "";
}

// void Imager::GetDetectorType(void) {
//     fDetectorType = fPm->GetStringParameter(GetFullParmName("DetectorType"));
// }

void Imager::GetCrystalMaterial(void) {
    fCrystalMaterial = fPm->GetStringParameter(GetFullParmName("Crystal/Material"));
}
G4VPhysicalVolume* Imager::Construct(void) {}

void Imager::CommonParameters(void) {
    G4cerr << "Start constructing " << fName << G4endl;
    GetCrystalShape();
    if (fCrystalShape=="Box") {
        GetCrystalHLX();
        GetCrystalHLY();
        GetCrystalHLZ();
    } else if (fCrystalShape=="Cylinder") {
        GetCrystalRadius();
        GetCrystalHL();
    } else {
        G4cerr << "Not supported crystal shape: "<<fCrystalShape << G4endl;
        throw std::runtime_error("Not supported crystal shape");
    }

    GetMaterial();
    GetNbOfXBins();
    GetNbOfYBins();
    GetNbOfZBins();

    IsCollimatorExists();
    GetCollimatorHLY();
    GetCollimatorMaterial();
    GetCollimatorOpeningMaterial();
    GetCrystalMaterial();

    if (fPm->ParameterExists(GetFullParmName("CheckForOverlapsResolution"))){
        resolution = fPm->GetIntegerParameter(GetFullParmName("CheckForOverlapsResolution"));
    }else{
        resolution = fPm->GetIntegerParameter("Ge/CheckForOverlapsResolution");
    }
    if (fPm->ParameterExists(GetFullParmName("CheckForOverlapsTolerance"))){
        tolerance = fPm->GetDoubleParameter(GetFullParmName("CheckForOverlapsTolerance"), "Length");
    }else{
        tolerance = fPm->GetDoubleParameter("Ge/CheckForOverlapsTolerance", "Length");
    }
}
