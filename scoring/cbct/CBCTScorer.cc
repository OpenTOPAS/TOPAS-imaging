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

CBCTScorer::CBCTScorer(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM,
    TsScoringManager* scM, TsExtensionManager* eM, G4String scorerName, G4String quantity,
    G4String outFileName, G4bool isSubScorer) :
    TsVBinnedScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer) {
    SetSurfaceScorer();
    SetUnit("");
    fOSF = fPm->ParameterExists(GetFullParmName("OpticalSpreadFunction/Enable"))
               ? fPm->GetBooleanParameter(GetFullParmName("OpticalSpreadFunction/Enable"))
               : false;

    fForcedInteraction = fPm->ParameterExists(GetFullParmName("ForceInteraction"))
                             ? fPm->GetBooleanParameter(GetFullParmName("ForceInteraction"))
                             : false;
    if (fForcedInteraction) {
        G4cout << "Enabled forced interaction" << G4endl;
    }
    if (fOSF) {
        G4cout << "Using optical spread function" << G4endl;
        fNumberOfEfficiency = fPm->GetVectorLength(
            GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Energies"));
        fEfficiencyEnergy = fPm->GetDoubleVector(
            GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Energies"), "Energy");
        fEfficiency = fPm->GetUnitlessVector(
            GetFullParmName("OpticalSpreadFunction/DetectorEfficiency/Efficiency"));
        xBinNum       = fPm->GetIntegerParameter(GetFullParmName("XBins"));
        yBinNum       = fPm->GetIntegerParameter(GetFullParmName("YBins"));
        kernel_size_u = fPm->GetIntegerParameter(GetFullParmName("OpticalSpreadFunction/KernelU"));
        kernel_size_v = fPm->GetIntegerParameter(GetFullParmName("OpticalSpreadFunction/KernelV"));
        E0            = fPm->GetDoubleParameter(
                 GetFullParmName("OpticalSpreadFunction/InitialEnergy"), "Energy") /
             keV;
        E_step =
            fPm->GetDoubleParameter(GetFullParmName("OpticalSpreadFunction/EnergyStep"), "Energy") /
            keV;

        G4int E_ind;
        for (int ee = 0; ee < fNumberOfEfficiency; ee++) {
            E_ind = (G4int) (((fEfficiencyEnergy[ee] / keV) - E0) / E_step);
            // std::cout << "Energy: " << fEfficiencyEnergy[ee] / keV << " Ind " << E_ind << " Efficiency " << fEfficiency[ee] << std::endl;
            Efficiency.insert(std::make_pair(E_ind, fEfficiency[ee]));
        }
        // ReadNumBins();
        // ReadPixelSpacings();
        std::cout << "kernel size ( " << kernel_size_u << ", " << kernel_size_v << " )"
                  << std::endl;
        kernel = (float**) malloc((kernel_size_u * 2 + 1) * sizeof(float*));
        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            kernel[kk] = (float*) malloc((kernel_size_v * 2 + 1) * sizeof(float));
            for (int ll = 0; ll < kernel_size_v * 2 + 1; ll++) {
                kernel[kk][ll] = 0.0;
            }
        }
        ReadOSFPath();
        ReadOSFData();
    } else {
        if (fPm->ParameterExists(GetFullParmName("DetectionEfficiency"))) {
            fDetectionEfficiency =
                fPm->GetUnitlessParameter(GetFullParmName("DetectionEfficiency"));
            std::cout << fDetectionEfficiency << std::endl;
        } else {
            fDetectionEfficiency = 0.05;
        }

        if (fPm->ParameterExists(GetFullParmName("DetectorGain"))) {
            fDetectorGain = fPm->GetUnitlessParameter(GetFullParmName("DetectorGain"));
            std::cout << fDetectorGain << std::endl;
        } else {
            fDetectorGain = 2.7e5;
            // std::cout << "DetectorGain is required" << std::endl;
            // exit(-1);
        }
    }
    srand(0);
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
        std::cout << OSFPath << std::endl;
    } else {
        std::cout << "The path for optical spread function are required" << std::endl;
        assert(false);
    }
}
void CBCTScorer::ReadOSFData(void) {
    std::cout << "Reading OSF files" << std::endl;
    // const std::filesystem::path path = OSFPath;
    std::vector<std::string> files;
    struct dirent*           entry;
    DIR*                     dir = opendir(OSFPath.c_str());
    int                      retn;
    std::string              filepath, extension;
    std::ifstream            fp;
    std::string              filename;
    G4double                 ee;
    G4int                    e_ind;
    if (dir == NULL) {
        std::cout << "Error" << std::endl;
    }
    size_t pos_s = 0, pos_e = 0;
    while ((entry = readdir(dir)) != NULL) {
        filepath = entry->d_name;
        // std::cout << filepath << std::endl;
        pos_e = filepath.find('.');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;
            pos_e = filepath.find('.', pos_s);
        }

        extension = filepath.substr(pos_s, pos_e);

        // retn = strcmp(extension.c_str(), "bin");
        if (strcmp(extension.c_str(), "bin") == 0) {
            files.push_back(OSFPath + "/" + entry->d_name);
            // std::cout << files.back() << std::endl;
        }
    }
    closedir(dir);
    // float *kernel1 = (float *)malloc((kernel_size_u * 2 + 1) * (kernel_size_v * 2 + 1) * sizeof(float));

    for (int ff = 0; ff < files.size(); ff++) {
        float** rkernel = (float**) malloc((kernel_size_u * 2 + 1) * sizeof(float*));
        for (int kk = 0; kk < kernel_size_u * 2 + 1; kk++) {
            rkernel[kk] = (float*) malloc((kernel_size_v * 2 + 1) * sizeof(float));
            for (int ll = 0; ll < kernel_size_v * 2 + 1; ll++) {
                rkernel[kk][ll] = 0.0;
            }
        }

        std::cout << files[ff] << std::endl;
        filepath = files[ff];
        pos_s    = 0;
        pos_e    = filepath.find('/');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;

            pos_e = filepath.find('/', pos_s);
        }
        filename = filepath.substr(pos_s, pos_e);
        pos_s    = 0;
        pos_e    = filename.find('.');
        while (pos_e != std::string::npos) {
            pos_s = pos_e + 1;
            pos_e = filename.find('.', pos_s);
        }
        ee    = std::stod(filename.substr(0, pos_s - 1));
        e_ind = (G4int) (ee - E0) / E_step;
        // std::cout << "energy " << ee << " Eind " << e_ind << std::endl;
        fp.open(files[ff].c_str(), std::ios::in | std::ios::binary);
        if (!fp) {
            std::cout << "Error: File couldn't be opened" << std::endl;
            exit(-1);
        }
        // fp.read((char *)(&kernel1[0]), (kernel_size_u * 2 + 1) * (kernel_size_v * 2 + 1) * sizeof(kernel1[0]));
        for (int row = 0; row < kernel_size_u * 2 + 1; row++) {   // stop loops if nothing to read
            fp.read((char*) (&rkernel[row][0]), (kernel_size_v * 2 + 1) * sizeof(float));
            if (!fp) {
                std::cout << "Error: Reading file for element " << row << std::endl;
                exit(-1);
            }
        }
        fp.close();

        kernels[e_ind] = rkernel;
        // kernels.insert(std::make_pair(e_ind, new float **(&rkernel)));
    }
    std::cout << "Reading OSF files done!" << std::endl;
}

G4bool CBCTScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    if (!fIsActive) {
        fSkippedWhileInactive++;
        return false;
    }
    if (fOSF) {
        ParticleEnergy = aStep->GetPreStepPoint()->GetKineticEnergy() / keV;
        index          = GetIndex(aStep);
        // xBin = GetBin(index, 0);
        // yBin = GetBin(index, 1);
        // index2 = fComponent->GetIndex(xBin, yBin, 0);
        // assert(index == index2);
        G4int    Eind1, Eind2;
        G4double E1, E2;
        Eind1  = (G4int) (std::floor((ParticleEnergy - E0) / E_step));
        Eind2  = (G4int) (std::ceil((ParticleEnergy - E0) / E_step));
        E1     = E0 + Eind1 * E_step;
        E2     = E0 + Eind2 * E_step;
        ef_low = Efficiency.lower_bound(Eind1);
        ef_up  = Efficiency.upper_bound(Eind2);
        if (std::fabs(ParticleEnergy - E1) < 1e-4) {
            weight1 = 1.0;
            weight2 = 0.0;
        } else if (std::fabs(ParticleEnergy - E2) < 1e-4) {
            weight1 = 0.0;
            weight2 = 1.0;
        } else {
            weight1 = (E2 - ParticleEnergy) / E_step;
            weight2 = (ParticleEnergy - E1) / E_step;
        }

        detection_efficiency = weight1 * ef_low->second + weight2 * ef_up->second;
        Rand                 = G4UniformRand();
        G4int ii, jj;
        G4int kernel_i, kernel_j;
        // std::cout << "process hit " << Rand << " Efficiency " << detection_efficiency << std::endl;
        // std::cout << "E " << ParticleEnergy << " E1 " << E1 << " weight1 " << weight1 << " E2 " << E2 << " weight2 " << weight2 << std::endl;
        k_low   = kernels.lower_bound(Eind1);
        k_up    = kernels.upper_bound(Eind2);
        kernel1 = k_low->second;
        kernel2 = k_up->second;
        for (int k_ii = 0; k_ii < kernel_size_u * 2 + 1; k_ii++) {
            for (int k_jj = 0; k_jj < kernel_size_v * 2 + 1; k_jj++) {
                kernel[k_ii][k_jj] = kernel1[k_ii][k_jj] * weight1 + kernel2[k_ii][k_jj] * weight2;
                // std::cout << "( " << k_ii << ", " << k_jj << " ): Kernel 1 " << kernel1[k_ii][k_jj] << " kernel 2 " << kernel2[k_ii][k_jj] << " kernel " << kernel[k_ii][k_jj] << std::endl;
            }
        }
        index = GetIndex(aStep);
        xBin  = GetBin(index, 0);
        yBin  = GetBin(index, 1);
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
                    index2   = fComponent->GetIndex(ii, jj, 0);
                    (*fEvtMap)[index2] += kernel[kernel_i][kernel_j] * detection_efficiency;
                }
            }
            l.unlock();
            // G4cout << "Index " << index << " Xbin " << xBin << " Ybin " << yBin << G4endl;
            // G4cout << "xLast " << xBinNum << " yLast " << yBinNum << G4endl;
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
            return true;
        } else {
            if (Rand <= detection_efficiency) {
                G4AutoLock l(&aMutex);
                // std::cout << "weight " << aStep->GetPreStepPoint()->GetWeight() << std::endl;
                // std::cout << "E " << ParticleEnergy << " E1 " << E1 << " weight1 " << weight1 << " E2 " << E2 << " weight2 " << weight2 << std::endl;
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
                        index2   = fComponent->GetIndex(ii, jj, 0);
                        AccumulateHit(aStep, kernel[kernel_i][kernel_j], index2);
                        // (*fEvtMap)[index2] += kernel[kernel_i][kernel_j];
                    }
                }
                l.unlock();

                // G4cout << "Index " << index << " Xbin " << xBin << " Ybin " << yBin << G4endl;
                // G4cout << "xLast " << xBinNum << " yLast " << yBinNum << G4endl;
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
            if (fForcedInteraction && weight != 0.) {
                AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain * fDetectionEfficiency);
                aStep->GetTrack()->SetTrackStatus(fStopAndKill);   // Kill track
                return true;
            } else {
                if (weight != 0. && rand() % 2 < fDetectionEfficiency) {
                    AccumulateHit(aStep, weight * 1.602e-19 * fDetectorGain);
                    return true;
                }
            }
        }
    }
    return false;
}