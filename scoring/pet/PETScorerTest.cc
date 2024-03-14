// Scorer for PETScorerTest
//
// Copyright 2020 Joseph Feld (feldj@mit.edu) and Gabriel Cojocaru (r_eality@mit.edu)
// All rights reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "PETScorerTest.hh"

#include "G4PSDirectionFlag.hh"
#include "globals.hh"

#include "EnergyCutoffModule.hh"
#include "TimeResolution.hh"
#include "DeadTime.hh"

#include "Coincidence.hh"

#include <cmath>

PETScorerTest::PETScorerTest(TsParameterManager *pM, TsMaterialManager *mM,
                             TsGeometryManager *gM, TsScoringManager *scM,
                             TsExtensionManager *eM, G4String scorerName,
                             G4String quantity, G4String outFileName,
                             G4bool isSubScorer)
    : DigitizerScorerCrystalOnly(pM, mM, gM, scM, eM, scorerName, quantity,
                                 outFileName, isSubScorer)
{
  InitializeOutputColumns();

  // FWHM of time blur
  fTimeResolution =
      fPm->GetDoubleParameter(GetFullParmName("TimeResolution"), "Time");

  fDeadTime =
      fPm->GetDoubleParameter(GetFullParmName("DeadTime"), "Time");

  // Max energy
  fMinEnergy =
      fPm->GetDoubleParameter(GetFullParmName("EnergyThreshold"), "Energy");

  // Min energy
  fMaxEnergy =
      fPm->GetDoubleParameter(GetFullParmName("EnergyUphold"), "Energy");
  G4cout << "Min energy: " << fMinEnergy << " , Max energy: " << fMaxEnergy << G4endl;
  // for coincidence detection
  fCoincidenceTimeWindowPrompts =
      fPm->GetDoubleParameter(GetFullParmName("CoincidenceTimeWindowPrompts"), "Time");
  G4cout << "Coincidence window: " << fCoincidenceTimeWindowPrompts / ns << " ns" << G4endl;
  fCoincidenceTimeWindowRandoms =
      fPm->GetDoubleParameter(GetFullParmName("CoincidenceTimeWindowRandoms"), "Time");

  fCoincidenceTimeDelayRandoms =
      fPm->GetDoubleParameter(GetFullParmName("CoincidenceTimeDelayRandoms"), "Time");

  fRunConsolidationFrequency =
      fPm->ParameterExists(GetFullParmName("RunConsolidationFrequency"))
          ? fPm->GetIntegerParameter(GetFullParmName("RunConsolidationFrequency"))
          : fNbRuns;

  fConsolidateRuns =
      fPm->ParameterExists(GetFullParmName("RunConsolidationFrequency"));

  fNbRuns = fPm->GetIntegerParameter("Tf/NumberOfSequentialTimes");
  runCount = 0;
  if (fIncludeCastorOutput)
  {
    fCastorIncludeTOF =
        fPm->ParameterExists(GetFullParmName("CastorIncludeTOF"))
            ? fPm->GetBooleanParameter(GetFullParmName("CastorIncludeTOF"))
            : false;

    fCastorTOFResolution =
        fCastorIncludeTOF
            ? fPm->GetDoubleParameter(GetFullParmName("CastorTOFResolution"), "Time")
            : 0;

    fCastorIncludeRandomsCorrection =
        fPm->ParameterExists(GetFullParmName("CastorIncludeRandomsCorrection"))
            ? fPm->GetBooleanParameter(GetFullParmName("CastorIncludeRandomsCorrection"))
            : false;
  }

  // the minimum distance between blocks for a detection to occur
  // in other systems, this is an integer number of crystals, but I don't have
  // an indexing system together for that
  fMinDistanceInCoincidence = fPm->GetDoubleParameter(
      GetFullParmName("MinDistanceInCoincidence"), "Length");

  G4AutoLock l(&aMutex);
  if (digitizers->empty())
  {
    digitizers->push_back(new EnergyCutoffModule(fMinEnergy, fMaxEnergy));
    digitizers->push_back(new TimeResolution(fTimeResolution));
    digitizers->push_back(new DeadTime(fDeadTime));
  }
  l.unlock();

  coincidenceCounterPrompts =
      new CoincidenceCounterDistance(fMinDistanceInCoincidence);

  coincidenceCounterRandoms =
      new CoincidenceCounterDistance(fMinDistanceInCoincidence);

  G4cerr << "PETScorer made!" << G4endl;
  G4cerr << "Current Number of Digitizers: " << digitizers->size() << G4endl;
  G4String castorOutputFilename = fCastorOutputFilename;
  if (fIncludeCastorOutput)
  {
    foutputFile.open(fCastorOutputFilename + ".cdf", std::ofstream::out);
  }
  fCoincidenceCounter = 0;
}

PETScorerTest::~PETScorerTest()
{
  delete coincidenceCounterPrompts;
  delete coincidenceCounterRandoms;
}

void PETScorerTest::InitializeOutputColumns()
{

  G4cout << "Initializing PET scorer columns" << G4endl;

  fNtuple->RegisterColumnI(&fEventID1, "EventID 1");

  // fNtuple->RegisterColumnS(&fName1, "Volume Name 1");
  fNtuple->RegisterColumnI(&fCrystalIndex1, "Crystal Index 1");
  fNtuple->RegisterColumnD(&fXOrigin1, "X Origin 1", "cm");
  fNtuple->RegisterColumnD(&fYOrigin1, "Y Origin 1", "cm");
  fNtuple->RegisterColumnD(&fZOrigin1, "Z Origin 1", "cm");
  fNtuple->RegisterColumnD(&fEnergy1, "Energy 1", "MeV");

  fNtuple->RegisterColumnD(&fXPos1, "X Position 1", "cm");
  fNtuple->RegisterColumnD(&fYPos1, "Y Position 1", "cm");
  fNtuple->RegisterColumnD(&fZPos1, "Z Position 1", "cm");
  fNtuple->RegisterColumnI(&fCrystalIndex1, "Crystal Index 1");
  fNtuple->RegisterColumnI(&fSubmoduleIndex1, "Submodule Index 1");
  fNtuple->RegisterColumnI(&fModuleIndex1, "Module Index 1");
  fNtuple->RegisterColumnI(&fDetectorIndex1, "Detector Index 1");
  fNtuple->RegisterColumnD(&fTime1, "Timestamp 1", "ms");
  // fNtuple->RegisterColumnD(&fCreationTime1, "Creation time 1", "ns");
  // fNtuple->RegisterColumnS(&fName2, "Volume Name 2");
  fNtuple->RegisterColumnI(&fEventID2, "EventID 2");

  fNtuple->RegisterColumnI(&fCrystalIndex2, "Crystal Index 2");
  fNtuple->RegisterColumnD(&fXOrigin2, "X Origin 2", "cm");
  fNtuple->RegisterColumnD(&fYOrigin2, "Y Origin 2", "cm");
  fNtuple->RegisterColumnD(&fZOrigin2, "Z Origin 2", "cm");
  fNtuple->RegisterColumnD(&fEnergy2, "Energy 2", "MeV");
  fNtuple->RegisterColumnD(&fXPos2, "X Position 2", "cm");
  fNtuple->RegisterColumnD(&fYPos2, "Y Position 2", "cm");
  fNtuple->RegisterColumnD(&fZPos2, "Z Position 2", "cm");
  fNtuple->RegisterColumnI(&fCrystalIndex2, "Crystal Index 2");
  fNtuple->RegisterColumnI(&fSubmoduleIndex2, "Submodule Index 2");
  fNtuple->RegisterColumnI(&fModuleIndex2, "Module Index 2");
  fNtuple->RegisterColumnI(&fDetectorIndex2, "Detector Index 2");
  // fNtuple->RegisterColumnD(&fCreationTime2, "Creation time 2", "ns");
  fNtuple->RegisterColumnD(&fTime2, "Timestamp 2", "ms");

  fNtuple->RegisterColumnD(&fTimeDelta, "Time Difference c1-c2", "ps");
  fNtuple->RegisterColumnD(&fRandomIntensityRate, "Random Intensity Rate sec/count", "s");
}

void PETScorerTest::FillOutputColumns(std::vector<Pulse> *pulsesSoFar)
{

  fCoincidencesPrompts.clear();
  fCoincidencesRandoms.clear();

  // Two time windows for prompt and random events
  CoincidenceDetection(pulsesSoFar, fCoincidencesPrompts, fCoincidencesRandoms, false, true);
  // CoincidenceDetection(pulsesSoFar, coincidencesRandoms, fCoincidenceTimeWindowRandoms, coincidenceCounterRandoms);
  fCoincidenceCounter += fCoincidencesPrompts.size();
  G4double minTime = std::numeric_limits<G4double>::max();
  G4double maxTime = std::numeric_limits<G4double>::min();
  G4double minTimeDelta = std::numeric_limits<G4double>::max();
  G4double maxTimeDelta = std::numeric_limits<G4double>::min();

  // get min/max times
  for (auto coincidence : fCoincidencesPrompts)
  {
    Pulse pulse1 = coincidence.pulse1;
    Pulse pulse2 = coincidence.pulse2;
    fTime = pulse1.eventStartTimeTopas;
    minTime = std::min(minTime, fTime);
    maxTime = std::max(maxTime, fTime);
  }

  fminTime = std::min(minTime, fminTime);
  fmaxTime = std::max(maxTime, fmaxTime);

  G4double randomIntensityRate = fCoincidencesRandoms.size() / (maxTime - minTime); //#counts/time
  if (fCoincidencesRandoms.size() == 0)
  {
    fRandomIntensityRate = 0;
  }
  else
  {
    fRandomIntensityRate = 1 / randomIntensityRate;
  }

  // now we have the iterator from the final digitizer
  for (auto coincidence : fCoincidencesPrompts)
  {
    Pulse pulse1 = coincidence.pulse1;
    Pulse pulse2 = coincidence.pulse2;
    fEventID1 = pulse1.eventID;

    G4ThreeVector position1 = pulse1.position;
    G4ThreeVector origin1 = pulse1.OriginPosition;
    // G4cout << pulse1.eventID << " " << pulse1.OriginPosition << " " << pulse2.eventID << " " << pulse2.OriginPosition << G4endl;
    fXPos1 = position1.x();
    fYPos1 = position1.y();
    fZPos1 = position1.z();

    fXOrigin1 = origin1.x();
    fYOrigin1 = origin1.y();
    fZOrigin1 = origin1.z();
    // fCreationTime1 = pulse1.creation_time;
    fEnergy1 = pulse1.energy;

    fName1 = pulse1.physicalVolumeName;
    fCrystalIndex1 = pulse1.crystalIndex;
    fSubmoduleIndex1 = pulse1.submoduleIndex;
    fModuleIndex1 = pulse1.moduleIndex;
    fDetectorIndex1 = pulse1.detectorIndex;

    fEventID2 = pulse2.eventID;
    G4ThreeVector position2 = pulse2.position;
    G4ThreeVector origin2 = pulse2.OriginPosition;

    fXPos2 = position2.x();
    fYPos2 = position2.y();
    fZPos2 = position2.z();
    fXOrigin2 = origin2.x();
    fYOrigin2 = origin2.y();
    fZOrigin2 = origin2.z();
    // fCreationTime2 = pulse2.creation_time;

    fEnergy2 = pulse2.energy;

    fName2 = pulse2.physicalVolumeName;
    fCrystalIndex2 = pulse2.crystalIndex;
    fSubmoduleIndex2 = pulse2.submoduleIndex;
    fModuleIndex2 = pulse2.moduleIndex;
    fDetectorIndex2 = pulse2.detectorIndex;

    fTime = pulse1.eventStartTimeTopas;
    fTimeDelta = pulse1.endTimeTopas - pulse2.endTimeTopas;

    if (fIncludeCastorOutput)
    {

      minTimeDelta = std::min(minTimeDelta, fTimeDelta);
      maxTimeDelta = std::max(maxTimeDelta, fTimeDelta);

      fTimeCastor = fTime / ms;
      // fAttenuationCorrectionFactorCastor
      // fUnnormalizedScatterIntensityCastor
      fUnnormalizedRandomIntensityCastor = randomIntensityRate * s;
      // fNormalizationFactorCastor
      fTimeDeltaCastor = fTimeDelta / ps;
      fCrystalID1Castor = fCrystalIndex1;
      fCrystalID2Castor = fCrystalIndex2;

      static_assert(sizeof(float) == 4,
                    "The float type expected to have 32 bits");

      // Might not work properly on all platforms
      foutputFile.write(reinterpret_cast<char *>(&fTimeCastor),
                        sizeof(uint32_t));
      if (fCastorIncludeRandomsCorrection)
      {
        foutputFile.write(reinterpret_cast<char *>(&fUnnormalizedRandomIntensityCastor),
                          sizeof(float));
      }

      if (fCastorIncludeTOF)
      {
        foutputFile.write(reinterpret_cast<char *>(&fTimeDeltaCastor),
                          sizeof(float));
      }
      foutputFile.write(reinterpret_cast<char *>(&fCrystalID1Castor),
                        sizeof(uint32_t));
      foutputFile.write(reinterpret_cast<char *>(&fCrystalID2Castor),
                        sizeof(uint32_t));
    }

    fminTimeDelta = std::min(minTimeDelta, fminTimeDelta);
    fmaxTimeDelta = std::max(maxTimeDelta, fmaxTimeDelta);
    // fOrigin1 = pulse1.Origin;
    // fOrigin2 = pulse2.Origin;
    fNtuple->Fill();
  }
}

void PETScorerTest::Output()
{
  DigitizerScorerCrystalOnly::Output();
  G4cout << "Output function call from PETScorer" << G4endl;
  if (fIncludeCastorOutput)
  {
    foutputFile.close();
  }
  // write out header file
  if (fIncludeCastorOutput)
  {
    std::ofstream header(fCastorOutputFilename + ".cdh");
    header << "Data filename: " << fCastorOutputFilename + ".cdf" << std::endl;
    header << "Number of events: " << fCoincidenceCounter << std::endl;
    header << "Data mode: "
           << "list-mode" << std::endl;
    header << "Data type: "
           << "PET" << std::endl;
    header << "Start time (s): " << fminTime / s << std::endl;
    header << "Duration (s): " << (fmaxTime - fminTime) / s << std::endl;
    header << "Scanner name: " << fCastorScannerName << std::endl;
    header << "Maximum number of lines per event: " << 1 << std::endl;
    header << "Calibration factor: " << fCastorCalibrationFactor << std::endl;
    header << "Attenuation correction flag: " << 0 << std::endl;
    header << "Normalization correction flag: " << 0 << std::endl;
    header << "Scatter correction flag: " << 0 << std::endl;
    header << "Random correction flag: " << fCastorIncludeRandomsCorrection << std::endl;

    if (fPm->ParameterExists(GetFullParmName("CastorIsotope")))
    {
      header << "Isotope: " << fCastorIsotope << std::endl;
    }

    header << "TOF information flag: " << fCastorIncludeTOF << std::endl;

    if (fCastorIncludeTOF)
    {
      header << "TOF resolution (ps): " << fCastorTOFResolution / ps << std::endl;
      header << "List TOF measurement range (ps): "
             << (fmaxTimeDelta - fminTimeDelta) / ps << std::endl;
    }
    header.close();
  }
  delete coincidenceCounterPrompts;
  delete coincidenceCounterRandoms;
}
void PETScorerTest::UserHookForEndOfRun()
{

  DigitizerScorerCrystalOnly::UserHookForEndOfRun();

  // G4AutoLock l(&aMutex);

  // // only garbage collect every fRunConsolidationFrequency runs
  // if (fConsolidateRuns && runCount % fRunConsolidationFrequency == 0)
  // {
  //   G4cout << "garbage collecting at run #" << runCount << "...";
  //   G4cout << "digitizing...";
  //   ApplyDigitizers();
  //   G4cout << "Done!"
  //          << " coincidence detection...";
  //   CoincidenceDetection(addedPulses, fCoincidencesPrompts, fCoincidencesRandoms, true, true);
  //   G4cout << "Done!" << G4endl;
  // }
  // // CoincidenceDetection(addedPulses, fCoincidencesPrompts, fCoincidencesRandoms, true, true);
  // // addedPulses->clear();

  // l.unlock();
  // runCount++;
  // if (runCount >= fNbRuns)
  // {
  //   G4cout << "deconstruction scorer" << G4endl;

  //   if (fIncludeCastorOutput)
  //   {
  //     foutputFile.close();
  //   }
  //   // write out header file
  //   if (fIncludeCastorOutput)
  //   {
  //     std::ofstream header(fCastorOutputFilename + ".cdh");
  //     header << "Data filename: " << fCastorOutputFilename + ".cdf" << std::endl;
  //     header << "Number of events: " << fCoincidenceCounter << std::endl;
  //     header << "Data mode: "
  //            << "list-mode" << std::endl;
  //     header << "Data type: "
  //            << "PET" << std::endl;
  //     header << "Start time (s): " << fminTime / s << std::endl;
  //     header << "Duration (s): " << (fmaxTime - fminTime) / s << std::endl;
  //     header << "Scanner name: " << fCastorScannerName << std::endl;
  //     header << "Maximum number of lines per event: " << 1 << std::endl;
  //     header << "Calibration factor: " << fCastorCalibrationFactor << std::endl;
  //     header << "Attenuation correction flag: " << 0 << std::endl;
  //     header << "Normalization correction flag: " << 0 << std::endl;
  //     header << "Scatter correction flag: " << 0 << std::endl;
  //     header << "Random correction flag: " << fCastorIncludeRandomsCorrection << std::endl;

  //     if (fPm->ParameterExists(GetFullParmName("CastorIsotope")))
  //     {
  //       header << "Isotope: " << fCastorIsotope << std::endl;
  //     }

  //     header << "TOF information flag: " << fCastorIncludeTOF << std::endl;

  //     if (fCastorIncludeTOF)
  //     {
  //       header << "TOF resolution (ps): " << fCastorTOFResolution / ps << std::endl;
  //       header << "List TOF measurement range (ps): "
  //              << (fmaxTimeDelta - fminTimeDelta) / ps << std::endl;
  //     }
  //     header.close();
  //   }
  //   delete coincidenceCounterPrompts;
  //   delete coincidenceCounterRandoms;
  // }
}

/*
void PETScorer::CoincidenceDetection(std::vector<Pulse>* singles,
                                     std::vector<Coincidence> &coincidencesPrompts,
                                     std::vector<Coincidence> &coincidencesRandoms,
                                     G4bool onlyDoStart, G4bool garbageCollect) {

  int firstBlockIndex = 0;
  std::vector<Pulse> windowPulses;

  for (int i = 0; i < singles->size(); ++i) {

    G4double timestampCurrent = (*singles)[i].endTimeTopas;
    G4double timestampFirst = (*singles)[firstBlockIndex].endTimeTopas;

    if (timestampCurrent - timestampFirst >
        fCoincidenceTimeWindowPrompts) { // outside time window
      coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulses);
      windowPulses.clear();
      firstBlockIndex = i;
    }
    windowPulses.push_back((*singles)[i]);
  }
  coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulses);

  G4cerr << "# coincidences: " << coincidencesPrompts.size() << G4endl;
}
*/

void PETScorerTest::CoincidenceDetection(std::vector<Pulse> *singles,
                                         std::vector<Coincidence> &coincidencesPrompts,
                                         std::vector<Coincidence> &coincidencesRandoms,
                                         G4bool onlyDoStart, G4bool garbageCollect)
{

  int firstBlockIndex = 0;
  int firstBlockIndexNew = 0;
  bool countPromptCoincidences = true;
  bool countRandomCoincidences = false;
  bool enteredRandomWindow = false;
  std::vector<Pulse> windowPulsesPrompts;
  std::vector<Pulse> windowPulsesRandoms;

  if (singles->size() == 0)
  {
    G4cout << "Singles list empty - skipping coincidence detection" << G4endl;
    return;
  }

  G4double newestTopasTime = singles->back().endTimeTopas;

  // the amount of time at the end of the singles list not to start a new window in
  G4double limitedTimeWindowMarginLength = 2 * fCoincidenceTimeDelayRandoms + 2 * fCoincidenceTimeWindowRandoms + 2 * fCoincidenceTimeWindowPrompts;

  // the latest time allowed to start a window at
  G4double maxTimeAllowed = newestTopasTime - limitedTimeWindowMarginLength;

  int lastIndexUsed = singles->size() - 1;

  int i = 0, j = 0;
  G4double timestampCurrent1;
  G4double timestampFirst1;
  G4int coincidence_id = -1;
  // G4cout << "Coincidence check for " << singles->size() << " singles" << G4endl;
  // for (int jj = 1; jj < singles->size(); jj++)
  // {
  //   assert((*singles)[jj].endTimeTopas >= (*singles)[jj - 1].endTimeTopas);
  // }
  // while (i < singles->size())
  // {
  //   timestampFirst1 = (*singles)[i].endTimeTopas;
  //   j = i + 1;
  //   while (j < singles->size())
  //   {
  //     timestampCurrent1 = (*singles)[j].endTimeTopas;
  //     if (timestampCurrent1 - timestampFirst1 < fCoincidenceTimeWindowPrompts)
  //     {
  //       if ((*singles)[i].eventID == (*singles)[j].eventID)
  //       {
  //         if (windowPulsesPrompts.size() == 0)
  //         {
  //           windowPulsesPrompts.push_back((*singles)[i]);
  //         }
  //         windowPulsesPrompts.push_back((*singles)[j]);
  //       }
  //       else
  //       {
  //         if (windowPulsesRandoms.size() == 0)
  //         {
  //           windowPulsesRandoms.push_back((*singles)[i]);
  //         }
  //         // windowPulsesRandoms.push_back((*singles)[i]);
  //         windowPulsesRandoms.push_back((*singles)[j]);
  //       }
  //     }
  //     else
  //     {
  //       // G4cout << "In" << G4endl;
  //       // coincidence_id = j;
  //       break;
  //     }
  //     // G4cout << "Time window: " << fCoincidenceTimeWindowPrompts << " ns temporal difference " << (timestampCurrent1 - timestampFirst1) << " ns " << (timestampCurrent1 - timestampFirst1 > fCoincidenceTimeWindowPrompts) << (timestampCurrent1 - timestampFirst1 < fCoincidenceTimeWindowPrompts) << G4endl;
  //     j++;
  //   }
  //   coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulsesPrompts);
  //   coincidenceCounterRandoms->AddCoincidences(coincidencesRandoms, windowPulsesRandoms);
  //   windowPulsesPrompts.clear();
  //   windowPulsesRandoms.clear();
  //   i = j;

  //   // if (j - i > 1)
  //   // {
  //   //   // G4cout << "Hit " << i << ", " << j << " num_coincidence " << j - i - 1 << G4endl;
  //   //   // G4cout << "eventID " << (*singles)[i].eventID << ", " << (*singles)[j - 1].eventID << G4endl;
  //   //   if ((*singles)[i].eventID == (*singles)[j - 1].eventID)
  //   //   {
  //   //     windowPulsesPrompts.push_back((*singles)[i]);
  //   //     windowPulsesPrompts.push_back((*singles)[j - 1]);
  //   //   }
  //   //   else
  //   //   {
  //   //     windowPulsesRandoms.push_back((*singles)[i]);
  //   //     windowPulsesRandoms.push_back((*singles)[j - 1]);
  //   //   }
  //   //   coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulsesPrompts);
  //   //   coincidenceCounterRandoms->AddCoincidences(coincidencesRandoms, windowPulsesRandoms);
  //   //   windowPulsesPrompts.clear();
  //   //   windowPulsesRandoms.clear();
  //   //   i = j + 1;
  //   // }
  //   // else
  //   // {
  //   //   i += 1;
  //   // }
  // }
  // G4cout << "Coincidence check done" << G4endl;

  i = 0;
  while (i < singles->size())
  {

    G4double timestampCurrent = (*singles)[i].endTimeTopas;
    G4double timestampFirst = (*singles)[firstBlockIndex].endTimeTopas;

    if (onlyDoStart && timestampFirst >= maxTimeAllowed)
    {
      G4cout << "ending early. rejecting " << (newestTopasTime - timestampFirst) / ns << " ns of margin" << G4endl;

      lastIndexUsed = std::max(i - 1, 0);
      break;
    }

    // outside prompt time window
    if (countPromptCoincidences &&
        timestampCurrent - timestampFirst > fCoincidenceTimeWindowPrompts)
    {
      /*
      if(firstBlockIndex == 0) {
        coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulsesPrompts);
      }
      */
      // if (windowPulsesPrompts.size() > 1)
      // {
      //   G4cout << "pulse in prompt" << G4endl;
      //   for (auto pulse : windowPulsesPrompts)
      //   {
      //     G4cout << "Pulse: " << pulse.endTimeTopas << " " << pulse.eventID << G4endl;
      //   }
      //   G4cout << "pulse in prompt end" << G4endl;
      // }
      coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulsesPrompts);
      windowPulsesPrompts.clear();
      firstBlockIndexNew = i;
      countPromptCoincidences = false;
    }

    // inside delayed randoms window for the first time
    if (!enteredRandomWindow && timestampCurrent - timestampFirst > fCoincidenceTimeDelayRandoms && timestampCurrent - timestampFirst < fCoincidenceTimeDelayRandoms + fCoincidenceTimeWindowRandoms)
    { // outside prompt time window

      windowPulsesRandoms.push_back((*singles)[firstBlockIndex]);
      enteredRandomWindow = true;
      countRandomCoincidences = true;
    }

    // left delayed randoms window
    if (timestampCurrent - timestampFirst > fCoincidenceTimeDelayRandoms + fCoincidenceTimeWindowRandoms)
    {
      firstBlockIndex = firstBlockIndexNew;
      countRandomCoincidences = false;
      enteredRandomWindow = false;
      // if (windowPulsesRandoms.size() > 1)
      // {
      //   G4cout << "pulse in random" << G4endl;
      //   for (auto pulse : windowPulsesRandoms)
      //   {
      //     G4cout << "Pulse: " << pulse.endTimeTopas << " " << pulse.eventID << G4endl;
      //   }
      //   G4cout << "pulse in random end" << G4endl;
      // }
      coincidenceCounterRandoms->AddCoincidences(coincidencesRandoms, windowPulsesRandoms);
      windowPulsesRandoms.clear();

      // loop back to find a new window start
      i = firstBlockIndexNew;
      countPromptCoincidences = true;
    }

    // record singles in correct place
    if (countPromptCoincidences)
    {
      windowPulsesPrompts.push_back((*singles)[i]);
    }
    if (countRandomCoincidences)
    {
      windowPulsesRandoms.push_back((*singles)[i]);
    }

    // At the end of the end of the list with a new block index to set
    if (i == singles->size() - 1 && firstBlockIndex != firstBlockIndexNew)
    {
      firstBlockIndex = firstBlockIndexNew;
      countRandomCoincidences = false;
      enteredRandomWindow = false;

      // loop back to find a new window start
      i = firstBlockIndexNew;
      countPromptCoincidences = true;

      // add first block single to the new list
      windowPulsesPrompts.push_back((*singles)[i]);
    }

    ++i;
  }

  if (windowPulsesPrompts.size() > 1)
  {
    G4cout << "pulse in prompt" << G4endl;
    for (auto pulse : windowPulsesPrompts)
    {
      G4cout << "Pulse: " << pulse.endTimeTopas << " " << pulse.eventID << G4endl;
    }
    G4cout << "pulse in prompt end" << G4endl;
  }
  if (windowPulsesRandoms.size() > 1)
  {
    G4cout << "pulse in random" << G4endl;
    for (auto pulse : windowPulsesRandoms)
    {
      G4cout << "Pulse: " << pulse.endTimeTopas << " " << pulse.eventID << G4endl;
    }
    G4cout << "pulse in random end" << G4endl;
  }
  coincidenceCounterPrompts->AddCoincidences(coincidencesPrompts, windowPulsesPrompts);
  coincidenceCounterRandoms->AddCoincidences(coincidencesRandoms, windowPulsesRandoms);

  G4cout << "# coincidences (prompt): " << coincidencesPrompts.size() << G4endl;
  G4cout << "# coincidences (random): " << coincidencesRandoms.size() << G4endl;

  // delete old singles that cannot generate new coincidences
  // if (garbageCollect)
  // {
  //   G4cout << "erasing " << lastIndexUsed << " singles...";
  //   singles->erase(singles->begin(), singles->begin() + lastIndexUsed);
  //   G4cout << "Done!" << G4endl;
  // }
}