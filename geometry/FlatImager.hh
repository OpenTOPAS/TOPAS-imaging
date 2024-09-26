// Copyright 2021 Hoyeon Lee (hlee80@mgh.harvard.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#pragma once
#include "Imager.hh"

#include "G4Colour.hh"

// #include "CrystalCounter.hh"
// #include "PETCrystalCounter.hh"
// #include "SPECTCrystalCounter.hh"

#include "TsVGeometryComponent.hh"

#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Visible.hh"
// #include "TsSequenceManager.hh"
// #include "TsParameterManager.hh"

class FlatImager : public Imager
{
public:
    FlatImager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM,
        TsGeometryManager* gM, TsVGeometryComponent* parentComponent,
        G4VPhysicalVolume* parentVolume, G4String& name);
    ~FlatImager();
    G4VPhysicalVolume* Construct();

protected:
    // Detector Parameters
    G4double  fCrystalRadi;
    G4double  fCrystalReflectorThickness;
    G4double  fCrystalRadiTotal;
    G4String  fReflectorMaterial;
    G4int     fNbPrefilters;
    G4double* fPrefilterThicknesses;
    G4double  fTotalPrefilterThicknesses;
    G4String* fPrefilterMaterials;
    G4double  fPhotoDetectorHLZ;
    G4String  fPhotoDetectorMaterial;
    G4int     fPtDtDivisionCount[3];

    // Functions for Detector Parameters
    void GetHLX(void);
    void GetHLY(void);
    void GetHLZ(void);
    void GetCrystalGapX(void);
    void GetCrystalGapY(void);
    void GetCrystalGapZ(void);
    void GetNbOfDetectors(void);
    void GetCrystalSize(void);

    void GetTubeMaterial(void);
    void GetCrystalRadi(void);
    void GetCrystalReflectorThickness(void);
    void GetReflectorMaterial(void);

    void GetPhotoDetectorXBins(void);
    void GetPhotoDetectorYBins(void);
    void GetPhotoDetectorZBins(void);
    // Functions for PET Paramaters
    void GetNbOfRings(void);

    void GetPrefilters(void);
    void ConstructPrefilters(G4VPhysicalVolume* DetectorPV, G4RotationMatrix* RotMatrix);
    void ConstructPhotodetector(G4VPhysicalVolume* DetectorPV, G4RotationMatrix* RotMatrix);

    void GetPhotoDetectorHLY(void);
    void GetPhotoDetectorMaterial(void);

    int      GetCrystalBoxID(int, int);
    G4double GetCrystalGapHLX(void);
    G4double GetCrystalGapHLY(void);
    G4double GetCrystalGapHLZ(void);
    G4double GetCollimatorOpeningHLX(void);
    G4double GetCollimatorOpeningHLY(void);

    G4bool   IsOnBoundary(G4ThreeVector, G4VSolid*, SurfaceType);
    G4double GetAreaOfSelectedSurface(G4VSolid*, SurfaceType, G4int, G4int, G4int);
    TsVGeometryComponent::SurfaceType GetSurfaceID(G4String);
    G4int                             GetIndex(G4Step* aStep);
    G4int                             GetIndex(G4int iX, G4int iY, G4int iZ);
    G4int                             GetBin(G4int index, G4int iBin);
    void NoteParameterizationError(G4double energy, G4String componentName, G4String volumeName);
    void NoteIndexError(
        G4double energy, G4String componentName, G4String coordinate, G4int value, G4int limit);
};
