// Copyright 2021 Hoyeon Lee (hlee80@mgh.harvard.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Visible.hh"
#include "TsVGeometryComponent.hh"

class Imager : public TsVGeometryComponent
{
public:
    Imager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
        TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name);
    ~Imager();
    G4VPhysicalVolume* Construct();
    void CommonParameters();

protected:
    // Imager type can be either "SPECT", "PET", "PG", "CBCT"
    // G4String fDetectorType;

    // Imager Parameters

    G4double fHLX;
    G4double fHLY;
    G4double fHLZ;
    G4String fMaterial;

    G4String fCrystalShape;
    G4double fCrystalHLX;
    G4double fCrystalHLY;
    G4double fCrystalHLZ;
    G4double fCrystalRadius;
    G4double fCrystalHL;
    G4double fCrystalGapX;
    G4double fCrystalGapY;
    G4double fCrystalGapZ;
    G4int    NbOfDetectors;
    G4int    NbOfXBins;
    G4int    NbOfYBins;
    G4int    NbOfZBins;

    // Collimator Parameters
    G4bool   fCollimatorExists;
    G4double fCollimatorHL;
    G4String fCollimatorOpeningMaterial;
    G4String fCollimatorMaterial;

    // PET Parameters
    G4double fMarginRings;
    G4int    NbOfRings;

    // Imager Materials
    G4String fCrystalMaterial;
    G4String fTubeMaterial;
    
    G4int resolution;
    G4double tolerance;

    // Ad-Hoc
    void   ColorLV(G4double, G4double, G4double, G4LogicalVolume*);
    void   ColorLV(G4Colour, G4LogicalVolume*);
    void   IsCollimatorExists(void);

    // Functions for Imager Parameters

    void GetHLX(void);
    void GetHLY(void);
    void GetHLZ(void);
    void GetMaterial(void);

    void     GetCrystalShape(void);
    void     GetCrystalRadius(void);
    void     GetCrystalHL(void);
    void     GetCrystalHLX(void);
    void     GetCrystalHLY(void);
    void     GetCrystalHLZ(void);
    void     GetNbOfXBins(void);
    void     GetNbOfYBins(void);
    void     GetNbOfZBins(void);
    G4double GetCrystalGapHLX(void);
    G4double GetCrystalGapHLZ(void);

    // Functions for SPECT Parameters
    G4double GetCollimatorOpeningHLX(void);
    G4double GetCollimatorOpeningHLY(void);
    G4double GetCollimatorOpeningHLZ(void);
    void     GetCollimatorHLY(void);

    // Functions for detector materials
    void GetCollimatorOpeningMaterial(void);
    void GetCollimatorMaterial(void);
    void GetCrystalMaterial(void);
    void GetTubeMaterial(void);

};
