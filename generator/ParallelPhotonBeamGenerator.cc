// Particle Generator for ParallelPhotonBeamGenerator
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "ParallelPhotonBeamGenerator.hh"

#include "G4Point3D.hh"
#include "G4RandomDirection.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh"
#include "TsParameterManager.hh"
#include "TsVGeometryComponent.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
//#include <CLHEP/Units/PhysicalConstants.h>
#include <cmath>

ParallelPhotonBeamGenerator::ParallelPhotonBeamGenerator(
    TsParameterManager *pM, TsGeometryManager *gM, TsGeneratorManager *pgM,
    G4String sourceName)
    : TsVGenerator(pM, gM, pgM, sourceName) {
  ResolveParameters();
}

void ParallelPhotonBeamGenerator::ResolveParameters() {
  TsVGenerator::ResolveParameters();

  fAngleError = fPm->GetDoubleParameter(GetFullParmName("AngleError"), "Angle");
  fMeanPositronRange =
      fPm->GetDoubleParameter(GetFullParmName("MeanPositronRange"), "Length");
}

// Creates a random unit vector error radians from the input vector
G4ThreeVector GetErrorVector(G4ThreeVector v, G4double error) {
  G4double deltaMag = tan(error);
  G4ThreeVector deltaVec = G4PlaneVectorRand(v).unit() * deltaMag;
  G4ThreeVector newDirection = (v + deltaVec).unit();

  return newDirection;
}

void ParallelPhotonBeamGenerator::GeneratePrimaries(G4Event *anEvent) {
  if (CurrentSourceHasGeneratedEnough())
    return;

  G4Point3D *componentPosition = fComponent->GetTransRelToWorld();

  // randomize starting positions based on mean positron range
  G4ThreeVector position =
      G4ThreeVector(componentPosition->x(), componentPosition->y(),
                    componentPosition->z()) +
      G4RandomDirection() * fMeanPositronRange;
      //+ G4UniformRand()*150*mm*G4ThreeVector(1,1,1).unit();//line source

  // direction of first photon
  G4ThreeVector firstDirection = G4RandomDirection();
  G4ThreeVector secondDirection = GetErrorVector(-firstDirection, fAngleError);

  // we define the direction of p1 and then base p2 off of that
  TsPrimaryParticle p1 = {.posX = (G4float)position.x(),
                          .posY = (G4float)position.y(),
                          .posZ = (G4float)position.z(),
                          .dCos1 = (G4float)firstDirection.x(),
                          .dCos2 = (G4float)firstDirection.y(),
                          .dCos3 = (G4float)firstDirection.z(),
                          .kEnergy = (G4float)fEnergy,
                          .weight = 1.,
                          .particleDefinition = fParticleDefinition,
                          .isNewHistory = true,
                          .isOpticalPhoton = fIsOpticalPhoton,
                          .isGenericIon = fIsGenericIon,
                          .ionCharge = fIonCharge};
  TsPrimaryParticle p2 = {.posX = (G4float)position.x(),
                          .posY = (G4float)position.y(),
                          .posZ = (G4float)position.z(),
                          .dCos1 = (G4float)secondDirection.x(),
                          .dCos2 = (G4float)secondDirection.y(),
                          .dCos3 = (G4float)secondDirection.z(),
                          .kEnergy = (G4float)fEnergy,
                          .weight = 1.,
                          .particleDefinition = fParticleDefinition,
                          .isNewHistory = false,
                          .isOpticalPhoton = fIsOpticalPhoton,
                          .isGenericIon = fIsGenericIon,
                          .ionCharge = fIonCharge};

  GenerateOnePrimary(anEvent, p1);
  GenerateOnePrimary(anEvent, p2);

  AddPrimariesToEvent(anEvent);
}
