// Scorer for SPECTScorer
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can 
// be found in the LICENSE file.

#include "SPECTScorer.hh"
#include "RotationBeginRun.hh"

#include "EnergyCutoffModule.hh"
#include "TimeResolution.hh"

#include <cassert>

SPECTScorer::SPECTScorer(TsParameterManager *pM, TsMaterialManager *mM,
                         TsGeometryManager *gM, TsScoringManager *scM,
                         TsExtensionManager *eM, G4String scorerName,
                         G4String quantity, G4String outFileName,
                         G4bool isSubScorer)
    : DigitizerScorerCrystalOnly(pM, mM, gM, scM, eM, scorerName, quantity,
                                 outFileName, isSubScorer) {
  InitializeOutputColumns();

  // FWHM of time blur
  fTimeResolution =
      fPm->GetDoubleParameter(GetFullParmName("TimeResolution"), "Time");

  // Max energy
  fMinEnergy =
      fPm->GetDoubleParameter(GetFullParmName("EnergyThreshold"), "Energy");

  // Min energy
  fMaxEnergy =
      fPm->GetDoubleParameter(GetFullParmName("EnergyUphold"), "Energy");

  NbOfDetectors =
      fPm->GetIntegerParameter(GetFullParmName("NumberOfDetectors"));

  G4AutoLock l(&aMutex);
  if (digitizers->empty()) {
    digitizers->push_back(new EnergyCutoffModule(fMinEnergy, fMaxEnergy));
    digitizers->push_back(new TimeResolution(fTimeResolution));
  }
  l.unlock();

  InitializeRotationAngleTracking();

  G4cout << "SPECTScorer made!" << G4endl;
}

SPECTScorer::~SPECTScorer() { ; }

void SPECTScorer::InitializeOutputColumns() {

  G4cout << "Initializing SPECT scorer columns" << G4endl;

  fNtuple->RegisterColumnI(&fCopyNr, "Crystal ID");
  fNtuple->RegisterColumnD(&fEnergy, "Energy", "MeV");
  fNtuple->RegisterColumnD(&fXPos, "X Position", "cm");
  fNtuple->RegisterColumnD(&fYPos, "Y Position", "cm");
  fNtuple->RegisterColumnD(&fZPos, "Z Position", "cm");
}

void SPECTScorer::FillOutputColumns(std::vector<Pulse>* pulsesSoFar) {

	G4cout << "lets do it" <<  G4endl;

  G4double minTime;
  G4double maxTime;

  G4int maxBin = 0;

  for (auto pulse : *pulsesSoFar) {
    G4int copyNr = pulse.crystalIndex;
    maxBin = std::max(maxBin, copyNr);
  }

  G4cout << "lets do it" <<  G4endl;

  G4cout << "biggitude 1: " << fRotationAngles.size() << G4endl;
  G4cout << "biggitude 2: " << maxBin + 1 << G4endl;

  // maps [projID][binID] to pair of time and num hits in that bin
  //std::pair<G4double, G4double> histogramBins[fRotationAngles.size()]
  //                                           [maxBin + 1];

  std::vector<std::vector<std::pair<G4double, G4double>>> histogramBins(fRotationAngles.size(),
  	                                                                    std::vector<std::pair<G4double, G4double>>(maxBin + 1,
  	                                                                    	                                         std::pair<G4double, G4double>()));

  //histogramBins

  for (auto pulse : *pulsesSoFar) {
  	G4cout << "lets do it p" <<  G4endl;

    G4ThreeVector position = pulse.position;
    fXPos = position.x();
    fYPos = position.y();
    fZPos = position.z();
    fEnergy = pulse.energy;
    fCopyNr = pulse.crystalIndex;

    if (fIncludeCastorOutput) {

      G4double pulseTime = pulse.endTimeTopas;

      minTime = std::min(minTime, pulseTime);
      maxTime = std::max(maxTime, pulseTime);

      histogramBins[pulse.projectionId][fCopyNr].first =
          std::min(pulseTime, histogramBins[pulse.projectionId][fCopyNr].first);
      histogramBins[pulse.projectionId][fCopyNr].second += 1;
    }

    fNtuple->Fill();
  }

  G4cout << "lets do it" <<  G4endl;

  // write out header file and binary file
  if (fIncludeCastorOutput) {

    std::ofstream outputFile;
    G4String castorOutputFilename = fCastorOutputFilename;

    outputFile.open(castorOutputFilename + ".cdf", std::ofstream::out);

    for (int i = 0; i < fRotationAngles.size(); ++i) {
      for (int j = 0; j <= maxBin; ++j) {
        fProjectionIdCastor = i;
        fBinIdCastor = j;
        fTimeCastor =
            histogramBins[fProjectionIdCastor][fBinIdCastor].first / ms;
        fDataInBin = histogramBins[fProjectionIdCastor][fBinIdCastor].second;

        outputFile.write(reinterpret_cast<char *>(&fTimeCastor),
                         sizeof(uint32_t));
        outputFile.write(reinterpret_cast<char *>(&fDataInBin), sizeof(float));
        outputFile.write(reinterpret_cast<char *>(&fProjectionIdCastor),
                         sizeof(uint32_t));
        outputFile.write(reinterpret_cast<char *>(&fBinIdCastor),
                         sizeof(uint32_t));
      }
    }

    outputFile.close();

    assert(fRotationAngles.size() >= 1);

    std::ofstream header(castorOutputFilename + ".cdh");
    header << "Data filename: " << castorOutputFilename + ".cdf" << std::endl;
    header << "Number of events: " << fRotationAngles.size() * (maxBin + 1)
           << std::endl;
    header << "Data mode: " << 1 << std::endl;
    header << "Data type: "
           << "SPECT" << std::endl;
    header << "Start time (s): "
           << fPm->GetDoubleParameter("Tf/TimelineStart", "Time") / s
           << std::endl;
    header << "Duration (s): "
           << fPm->GetDoubleParameter("Tf/TimelineEnd", "Time") / s
           << std::endl;
    header << "Scanner name: " << fCastorScannerName << std::endl;
    header << "Number of projections: "
           << fRotationAngles.size() * NbOfDetectors << std::endl;
    header << "Calibration factor: " << fCastorCalibrationFactor << std::endl;
    header << "Normalization correction flag: " << 0 << std::endl;
    header << "Scatter correction flag: " << 0 << std::endl;
    header << "Head rotation direction: "
           << "CW" << std::endl;

    if (fPm->ParameterExists(GetFullParmName("CastorIsotope"))) {
      header << "Isotope: " << fCastorIsotope << std::endl;
    }

    // add projection angles

    header << "Projection angles: ";

    header << fRotationAngles[0] / deg;

    for (int i = 1; i < fRotationAngles.size(); i++) {
      header << ", " << fRotationAngles[i] / deg;
    }

    header << std::endl;

    header.close();
  }
}
