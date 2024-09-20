// Scorer for CBCTScorer
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

#include <iostream>

#include "CBCTScorer.hh"
#include "G4PSDirectionFlag.hh"

G4Mutex CBCTScorer::aMutex = G4MUTEX_INITIALIZER;

CBCTScorer::CBCTScorer(TsParameterManager *pM, TsMaterialManager *mM, TsGeometryManager *gM, TsScoringManager *scM,
                       TsExtensionManager *eM, G4String scorerName, G4String quantity, G4String outFileName,
                       G4bool isSubScorer) :
        TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer) {
    SetSurfaceScorer();
    SetUnit("");
    fScoreSqure = fPm->ParameterExists(GetFullParmName("ScoreSquare")) ? fPm->GetBooleanParameter(
            GetFullParmName("ScoreSquare")) : false;
    fScoreCount = fPm->ParameterExists(GetFullParmName("ScoreCount")) ? fPm->GetBooleanParameter(
            GetFullParmName("ScoreCount")) : false;
    if (fScoreSqure && fScoreCount) {
        G4cerr << "Topas is exiting due to a serious error in CBCTScorer." << G4endl;
        G4cerr << "ScoreSquare and ScoreCount cannot be used together" << std::endl;
        exit(1);
    }
    fOSF = fPm->ParameterExists(GetFullParmName("OpticalSpreadFunction/Enable"))
           ? fPm->GetBooleanParameter(GetFullParmName("OpticalSpreadFunction/Enable"))
           : false;

    fForcedInteraction = fPm->ParameterExists(GetFullParmName("ForceInteraction"))
                         ? fPm->GetBooleanParameter(GetFullParmName("ForceInteraction"))
                         : false;
    if (fForcedInteraction) {
        G4cout << "CBCTScorer::Enabled forced interaction" << G4endl;
    }
    if (fOSF) {
        G4cout << "Using optical spread function" << G4endl;
        fNumberOfEfficiency =
                fPm->GetVectorLength(GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Energies"));
        fEfficiencyEnergy =
                fPm->GetDoubleVector(GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Energies"), "Energy");
        fEfficiency = fPm->GetUnitlessVector(GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Efficiency"));
        xBinNum = fPm->GetIntegerParameter(GetFullParmName("XBins"));
        yBinNum = fPm->GetIntegerParameter(GetFullParmName("YBins"));
        kernel_size_u = fPm->GetIntegerParameter(GetFullParmName("OpticalSpreadFunction/KernelU"));
        kernel_size_v = fPm->GetIntegerParameter(GetFullParmName("OpticalSpreadFunction/KernelV"));

        G4int E_ind;
        for (int ee = 0; ee < fNumberOfEfficiency; ee++) {
            if (fVerbosity > 0) {
                G4cout << "Energy: " << fEfficiencyEnergy[ee] << " Ind " << E_ind << " Efficiency " << fEfficiency[ee]
                       << G4endl;
            }
            Efficiency.insert(std::make_pair(fEfficiencyEnergy[ee], fEfficiency[ee]));
        }
        if (fVerbosity > 0) {
            G4cout << "kernel size ( " << kernel_size_u * 2 + 1 << ", " << kernel_size_v * 2 + 1 << " )" << G4endl;
        }
        kernel = (float **) malloc((kernel_size_u * 2 + 1) * sizeof(float *));
        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            kernel[kk] = (float *) malloc((kernel_size_v * 2 + 1) * sizeof(float));
            for (int ll = 0; ll < kernel_size_v * 2 + 1; ll++) {
                kernel[kk][ll] = 0.0;
            }
        }
        ReadOSFPath();
        ReadOSFData();
    } else {
        if (fPm->ParameterExists(GetFullParmName("DetectionEfficiency"))) {
            fDetectionEfficiency = fPm->GetUnitlessParameter(GetFullParmName("DetectionEfficiency"));
        } else {
            fDetectionEfficiency = 0.05;
        }

        if (fPm->ParameterExists(GetFullParmName("DetectorGain"))) {
            fDetectorGain = fPm->GetUnitlessParameter(GetFullParmName("DetectorGain"));
        } else {
            fDetectorGain = 2.7e5;
        }
    }
}

CBCTScorer::~CBCTScorer() {
    if (fOSF) {
        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            free(kernel[kk]);
        }
        free(kernel);

        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            free(rkernel[kk]);
        }
        free(rkernel);
    }
}

void CBCTScorer::ReadOSFPath(void) {
    if (fPm->ParameterExists(GetFullParmName("OpticalSpreadFunction/OSFPath"))) {
        OSFPath = fPm->GetStringParameter(GetFullParmName("OpticalSpreadFunction/OSFPath"));
        if (fVerbosity > 0) {
            G4cout << "Reading OSF from: " << OSFPath << G4endl;
        }
    } else {
        G4cerr << "Topas is exiting due to a serious error in CBCTScorer." << G4endl;
        G4cerr << "The path for optical spread function is required" << std::endl;
        exit(1);
    }
}

void CBCTScorer::ReadOSFData(void) {
    if (fVerbosity > 0) {
        G4cout << "CBCTScorer::Reading OSF files" << G4endl;
    }
    std::vector <std::string> files;
    struct dirent *entry;
    DIR *dir = opendir(OSFPath.c_str());
    int retn;
    std::string filepath, extension;
    std::ifstream fp;
    std::string filename;
    G4double ee;
    G4int e_ind;
    if (dir == NULL) {
        G4cerr << "Topas is exiting due to a serious error in CBCTScorer." << G4endl;
        G4cerr << "OSF directory cannot be opened" << G4endl;
        exit(1);
    }
    size_t pos_s = 0, pos_e = 0;
    while ((entry = readdir(dir)) != NULL) {
        filepath = entry->d_name;
        pos_e = filepath.find('.');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;
            pos_e = filepath.find('.', pos_s);
        }

        extension = filepath.substr(pos_s, pos_e);
        if (strcmp(extension.c_str(), "bin") == 0) {
            files.push_back(OSFPath + "/" + entry->d_name);
            if (fVerbosity > 3) {
                G4cout << files.back() << G4endl;
            }
        }
    }
    closedir(dir);
    if (fVerbosity > 0) {
        G4cout << files.size() << " files found" << G4endl;
    }
    for (int ff = 0; ff < files.size(); ff++) {
        float **rkernel = (float **) malloc((kernel_size_u * 2 + 1) * sizeof(float *));
        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            rkernel[kk] = (float *) malloc((kernel_size_v * 2 + 1) * sizeof(float));
            for (int ll = 0; ll < kernel_size_v * 2 + 1; ll++) {
                rkernel[kk][ll] = 0.0;
            }
        }

        if (fVerbosity > 3) {
            G4cout << files[ff] << G4endl;
        }
        filepath = files[ff];
        pos_s = 0;
        pos_e = filepath.find('/');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;

            pos_e = filepath.find('/', pos_s);
        }
        filename = filepath.substr(pos_s, pos_e);
        pos_s = 0;
        pos_e = filename.find('.');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;
            pos_e = filename.find('.', pos_s);
        }
        ee = std::stod(filename.substr(0, pos_s - 1)) * keV;
        double *lower = std::lower_bound(fEfficiencyEnergy, fEfficiencyEnergy + fNumberOfEfficiency, ee);
        int lower_ind = lower - fEfficiencyEnergy;
        if (fVerbosity > 0) {
            G4cout << "CBCTScorer::Energy " << ee << " Eind " << lower_ind << G4endl;
        }
        fp.open(files[ff].c_str(), std::ios::in | std::ios::binary);
        if (!fp) {
            G4cerr << "Topas is exiting due to a serious error in CBCTScorer." << G4endl;
            G4cerr << " OSF file couldn't be opened" << G4endl;
            exit(1);
        }
        // fp.read((char *)(&kernel1[0]), (kernel_size_u * 2 + 1) * (kernel_size_v * 2 + 1) * sizeof(kernel1[0]));
        for (int row = 0; row < kernel_size_u * 2 + 1; row++) {   // stop loops if nothing to read
            fp.read((char *) (&rkernel[row][0]), (kernel_size_v * 2 + 1) * sizeof(float));
            if (!fp) {
                G4cerr << "Topas is exiting due to a serious error in CBCTscorer." << G4endl;
                G4cerr << "Error: Reading file for element " << row << G4endl;
                exit(1);
            }
            if (fVerbosity > 3) {
                for (int llll = 0; llll < kernel_size_v * 2 + 1; llll++) {
                    G4cout << rkernel[row][llll] << " ";
                }
                G4cout << G4endl;
            }
        }
        fp.close();
        kernels[lower_ind] = rkernel;
    }
    if (fVerbosity > 0) {
        G4cout << "CBCTScorer::Reading OSF files done!" << G4endl;
    }
}

G4bool CBCTScorer::ProcessHits(G4Step *aStep, G4TouchableHistory *) {
    if (!fIsActive) {
        fSkippedWhileInactive++;
        return false;
    }
    ResolveSolid(aStep);
    G4double weight = aStep->GetPreStepPoint()->GetWeight();
    if (IsSelectedSurface(aStep)) {
        if (weight > 0.0) {
            if (fOSF) {
                ParticleEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
                index = GetIndex(aStep);
                if (ParticleEnergy < fEfficiencyEnergy[0]) {
                    return false;
                }
                G4double E1, E2;
                double *lower = std::lower_bound(fEfficiencyEnergy, fEfficiencyEnergy + fNumberOfEfficiency,
                                                 ParticleEnergy);
                int upper_ind = lower - fEfficiencyEnergy;
                int lower_ind = upper_ind - 1;
                E1 = fEfficiencyEnergy[lower_ind];
                E2 = fEfficiencyEnergy[upper_ind];
                ef_low = Efficiency[E1];
                ef_up = Efficiency[E2];
                assert(E2 >= ParticleEnergy && E1 <= ParticleEnergy);
                if (std::fabs(ParticleEnergy - E1) < 1e-4) {
                    weight1 = 1.0;
                    weight2 = 0.0;
                } else if (std::fabs(ParticleEnergy - E2) < 1e-4) {
                    weight1 = 0.0;
                    weight2 = 1.0;
                } else {
                    weight1 = (E2 - ParticleEnergy) / (E2 - E1);
                    weight2 = (ParticleEnergy - E1) / (E2 - E1);
                }
                detection_efficiency = weight1 * ef_low + weight2 * ef_up;
                Rand = G4UniformRand();
                G4int ii, jj;
                G4int kernel_i, kernel_j;

                if (fVerbosity > 0) {
                    G4cout << "E: " << ParticleEnergy << " E1: " << E1 << " E2: " << E2 << " ef: " << ef_low << " "
                           << ef_up
                           << G4endl;
                    G4cout << "lower: " << lower_ind << " upper: " << upper_ind << G4endl;
                    G4cout << "process hit " << Rand << " Efficiency " << detection_efficiency << G4endl;
                }
                kernel1 = kernels[lower_ind];
                kernel2 = kernels[upper_ind];
                for (int k_ii = 0; k_ii < kernel_size_u * 2 + 1; k_ii++) {
                    for (int k_jj = 0; k_jj < kernel_size_v * 2 + 1; k_jj++) {
                        kernel[k_ii][k_jj] = kernel1[k_ii][k_jj] * weight1 + kernel2[k_ii][k_jj] * weight2;
                    }
                }
                index = GetIndex(aStep);
                xBin = GetBin(index, 0);
                yBin = GetBin(index, 1);
                zBin = GetBin(index, 2);
                if (fVerbosity > 5) {
                    G4ThreeVector pos1 = aStep->GetPreStepPoint()->GetPosition();
                    G4ThreeVector pos2 = aStep->GetPostStepPoint()->GetPosition();
                    G4cout << "xPos1 " << pos1.x() << " yPos1 " << pos1.y() << " zPos1 " << pos1.z() << G4endl;
                    G4cout << "xPos2 " << pos2.x() << " yPos2 " << pos2.y() << " zPos2 " << pos2.z() << G4endl;
                    G4cout << "xBin " << xBin << " yBin " << yBin << " zBin " << zBin << G4endl;
                }
                if (fForcedInteraction) {
                    G4AutoLock l(&aMutex);
                    for (int i = -kernel_size_u; i < kernel_size_u; i++) {
                        ii = xBin + i;
                        if (ii < 0 || ii >= xBinNum) {   //Exclude
                            continue;
                        }
                        kernel_i = i + kernel_size_u;
                        for (int j = -kernel_size_v; j < kernel_size_v; j++) {
                            jj = yBin + j;
                            if (jj < 0 || jj >= yBinNum) {   // exclude
                                continue;
                            }
                            kernel_j = j + kernel_size_v;
                            index2 = fComponent->GetIndex(ii, jj, zBin);
                            if (fScoreSqure) {
                                AccumulateHit(aStep, kernel[kernel_i][kernel_j] * weight * detection_efficiency *
                                                     kernel[kernel_i][kernel_j] * weight * detection_efficiency,
                                              index2);
                            } else if (fScoreCount) {
                                AccumulateHit(aStep, 1, index2);
                            } else {
                                AccumulateHit(aStep, kernel[kernel_i][kernel_j] * weight * detection_efficiency,
                                              index2);
                            }
                        }
                    }
                    l.unlock();
                    aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
                    return true;
                } else {
                    if (Rand <= detection_efficiency) {
                        G4AutoLock l(&aMutex);
                        for (int i = -kernel_size_u; i < kernel_size_u; i++) {
                            ii = xBin + i;
                            if (ii < 0 || ii >= xBinNum) {   //Exclude
                                continue;
                            }
                            kernel_i = i + kernel_size_u;
                            for (int j = -kernel_size_v; j < kernel_size_v; j++) {
                                jj = yBin + j;
                                if (jj < 0 || jj >= yBinNum) {   // exclude
                                    continue;
                                }
                                kernel_j = j + kernel_size_v;
                                index2 = fComponent->GetIndex(ii, jj, zBin);
                                if (fScoreSqure) {
                                    AccumulateHit(aStep,
                                                  kernel[kernel_i][kernel_j] * weight * kernel[kernel_i][kernel_j] *
                                                  weight, index2);
                                } else if (fScoreCount) {
                                    AccumulateHit(aStep, 1, index2);
                                } else {
                                    AccumulateHit(aStep, kernel[kernel_i][kernel_j] * weight, index2);
                                }
                            }
                        }
                        l.unlock();
                        aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
                        return true;
                    } else {
                        return false;
                    }
                }
            } else {
                ResolveSolid(aStep);
                if (IsSelectedSurface(aStep)) {
                    G4double weight = aStep->GetPreStepPoint()->GetWeight();
                    if (fForcedInteraction && weight > 0.) {
                        if (fScoreSqure) {
                            AccumulateHit(aStep, (weight * 1.602e-19 * fDetectorGain * fDetectionEfficiency) *
                                                 (weight * 1.602e-19 * fDetectorGain * fDetectionEfficiency));
                        } else if (fScoreCount) {
                            AccumulateHit(aStep, 1);
                        } else {
                            AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain * fDetectionEfficiency);
                        }
                        aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
                        return true;
                    } else {
                        if (weight > 0. && G4UniformRand() < fDetectionEfficiency) {
                            if (fScoreSqure) {
                                AccumulateHit(aStep, (weight * 1.602e-19 * fDetectorGain) *
                                                     (weight * 1.602e-19 * fDetectorGain));
                            } else if (fScoreCount) {
                                AccumulateHit(aStep, 1);
                            } else {
                                //// Default
                                AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain);
                            }
                            aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
