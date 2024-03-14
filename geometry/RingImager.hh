// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
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
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Visible.hh"

class RingImager : public Imager
{
public:
    RingImager(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM,
        TsGeometryManager* gM, TsVGeometryComponent* parentComponent,
        G4VPhysicalVolume* parentVolume, G4String& name);
    ~RingImager();
    G4VPhysicalVolume* Construct();

protected:
    void ConstructDetector(void);
    void ConstructModules(G4VPhysicalVolume* DetectorPV, G4VPhysicalVolume* CollimatorPV);
    void ConstructSubmodules(G4VPhysicalVolume* ModulePV, G4ThreeVector* TransVectorModule,
        G4VPhysicalVolume* CollimatorPV);
    void ConstructCrystals(G4VPhysicalVolume* SubmodulePV, G4ThreeVector* TransVectorSubmodule,
        G4VPhysicalVolume* CollimatorPV);
    // Detector type can be either "SPECT", "PET" or "PG"
    G4String fDetectorType;
    // Detector Parameters
    G4double fRingRadius;
    // G4double fHLX;
    // G4double fHLY;
    // G4double fHLZ;

    G4double fModuleHLX;
    G4double fModuleHLY;
    G4double fModuleHLZ;

    G4double fSubmoduleHLX;
    G4double fSubmoduleHLY;
    G4double fSubmoduleHLZ;

    // G4double fCrystalHLX;
    // G4double fCrystalHLY;
    // G4double fCrystalHLZ;

    // G4int NbOfDetectors;
    // G4int NbOfXBins;
    // G4int NbOfYBins;
    // G4int NbOfZBins;

    G4int NbOfModulesX;
    G4int NbOfModulesY;
    G4int NbOfModulesZ;

    G4int NbOfSubmodulesX;
    G4int NbOfSubmodulesY;
    G4int NbOfSubmodulesZ;

    // SPECT & PG Parameters
    // G4double fCollimatorHLY;

    // PET Parameters
    G4double fMarginRings;
    G4double fSubmoduleGapX;
    G4double fSubmoduleGapY;
    G4double fSubmoduleGapZ;
    G4double fModuleGapX;
    G4double fModuleGapY;
    G4double fModuleGapZ;
    G4double fCrystalGapX;
    G4double fCrystalGapY;
    G4double fCrystalGapZ;

    // G4int NbOfRings;

    // Detector Materials
    // G4bool   fCollimatorExists;
    // G4String fCollimatorOpeningMaterial;
    // G4String fCollimatorMaterial;
    // G4String fCrystalMaterial;
    // G4String fTubeMaterial;

    // Castor Parameters
    G4String fScannerName;
    G4String fGeomPath;

    // Castor PET Parameters
    G4String fDescription;
    G4double fMinAngleDifference;
    G4double fMeanDepthOfInteraction;
    G4double fVoxelsTransaxial;
    G4double fVoxelsAxial;
    G4double fFOVTransaxial;
    G4double fFOVAxial;

    std::vector<G4int> moduleIDs;
    std::vector<G4int> submoduleIDs;
    std::vector<G4int> crystalIDs;
    std::vector<G4int> collimatorIDs;

    G4bool   fSaveCrystalCoordinate;
    G4String fCrystalCoordinateFilename;
    void     GetCrystalCoordinateSave(void);
    void     SaveCrystalCoordinate(void);

    // CrystalCounter* fCrystalCounter;

    int NbOfModulesPerRing;
    int NbOfModulesPerLayer;

    int fAxialRingSectorID;
    int fRingSectorID;
    int fAxialModuleID;
    int fTransaxialModuleID;
    int fLayerModuleID;
    int fBlockID;
    int fAxialSubmoduleID;
    int fLayerSubmoduleID;
    int fTransaxialSubmoduleID;
    int fSubmoduleID;
    int fAxialCrystalID;
    int fLayerCrystalID;
    int fTransaxialCrystalID;

    int fCrystalID;
    int fRingID;

    int NbOfAxialModules;
    int NbOfLayerModules;
    int NbOfTransaxialModules;
    int NbOfModulesPerDetector;
    int NbOfAxialSubmodules;
    int NbOfLayerSubmodules;
    int NbOfTransaxialSubmodules;
    int NbOfSubmodulesPerModule;
    int NbOfTransaxialSubmodulesPerRSector;
    int NbOfSubmodulesPerRing;
    int NbOfSubmodulesPerLayer;
    int NbOfSubmodulesPerAxialModule;
    int NbOfSubmodulesPerLayerModule;

    int NbOfAxialCrystals;
    int NbOfLayerCrystals;
    int NbOfTransaxialCrystals;
    int NbOfTransaxialCrystalPerRSector;
    int NbOfTransaxialCrystalPerModule;
    int NbOfCrystalsPerSubmodule;
    int NbOfCrystalsPerRing;
    int NbOfCrystalsPerLayer;
    int NbOfTransaxialCrystalPerAxialSubmodule;
    int NbOfTransaxialCrystalPerLayerSubmodule;
    int NbOfTransaxialCrystalPerAxialModule;
    int NbOfTransaxialCrystalPerLayerModule;

    // Functions for Detector Parameters
    // void GetDetectorType(void);
    // void GetNbOfXBins(void);
    // void GetNbOfYBins(void);
    // void GetNbOfZBins(void);
    // void GetCrystalHLX(void);
    // void GetCrystalHLY(void);
    // void GetCrystalHLZ(void);

    void GetNbOfModulesX(void);
    void GetNbOfModulesY(void);
    void GetNbOfModulesZ(void);

    void GetNbOfSubmodulesX(void);
    void GetNbOfSubmodulesY(void);
    void GetNbOfSubmodulesZ(void);

    void GetNbOfDetectors(void);
    void GetModuleGapHLX(void);
    void GetModuleGapHLY(void);
    void GetModuleGapHLZ(void);
    void GetSubmoduleGapHLX(void);
    void GetSubmoduleGapHLY(void);
    void GetSubmoduleGapHLZ(void);
    void GetCrystalGapHLX(void);
    void GetCrystalGapHLY(void);
    void GetCrystalGapHLZ(void);
    void GetRingRadius(void);
    void GetModuleHLX(void);
    void GetModuleHLY(void);
    void GetModuleHLZ(void);
    void GetSubmoduleHLX(void);
    void GetSubmoduleHLY(void);
    void GetSubmoduleHLZ(void);

    void GetHLX(void);
    void GetHLY(void);
    void GetHLZ(void);

    // Functions for PET Paramaters
    void GetNbOfRings(void);
    void GetMarginRings(void);

    // Functions for SPECT Parameters
    // G4double GetCollimatorOpeningHLX(void);
    // G4double GetCollimatorOpeningHLZ(void);
    // void     GetCollimatorHLY(void);

    // Detector specific helper functions
    G4double GetAngle(G4int detectorID);
    // CrystalCounter* GetCrystalCounter(void);
    // PETCrystalCounter* GetCrystalCounter(void);

    // Functions for detector materials
    // void GetCollimatorOpeningMaterial(void);
    // void GetCollimatorMaterial(void);
    // void GetCrystalMaterial(void);
    void GetTubeMaterial(void);

    // Creation of Castor Header
    void     CreateCastorHeader(void);
    G4String WriteCopies(G4String value, G4int numCopies);
    G4String WriteCopies(G4int value, G4int numCopies);
    G4String WriteCopies(G4double value, G4int numCopies);

    // Castor functions for Parameters
    void GetCastorScannerName(void);
    void GetCastorGeomPath(void);

    // Castor functions for PET Parameters
    void GetCastorDescription(void);
    void GetCastorMinAngleDifference(void);
    void GetCastorMeanDepthOfInteraction(void);
    void GetCastorOutputSize(void);
    void GetCastorOutputDepth(void);
    void GetCastorFOVSize(void);
    void GetCastorFOVDepth(void);

    void             SetCrystalID(int, int, int);
    int              GetCrystalBoxID(int, int, int);
    void             SetSubmoduleID(int, int, int);
    int              GetSubmoduleID(int, int, int);
    void             SetModuleID(int, int, int);
    int              GetModuleID(int, int, int);
    void             SetCurrentDetector(int);
    void             SetCrystalCounter(void);
    G4VSolid*        DetectorSolid;
    G4LogicalVolume* DetectorLV;

    G4VSolid*        ModuleSolid;
    G4LogicalVolume* ModuleLV;

    G4VSolid*        SubmoduleSolid;
    G4LogicalVolume* SubmoduleLV;

    G4VSolid*        CrystalSolid;
    G4LogicalVolume* CrystalLV;

    G4VSolid*           CollimatorDetectorSolid;
    G4LogicalVolume*    CollimatorDetectorLV;
    G4VPhysicalVolume*  CollimatorDetectorPV;
    G4VSolid*           CollimatorOpeningSolid;
    G4LogicalVolume*    CollimatorOpeningLV;
    G4VPhysicalVolume** DetectorPVs;
    G4VPhysicalVolume** ModulePVs;
    G4VPhysicalVolume** SubmodulePVs;
    G4VPhysicalVolume** CrystalPVs;
};
