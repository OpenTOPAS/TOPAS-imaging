# SPECT | PET | Prompt Gamma | CBCT 

Extentions used to implement SPECT, PET, Prompt Gamma, and CBCT in OpenTOPAS.

# Getting Started

## Tools required

This code is an extension for OpenTOPAS for simulating imaging systems that gives output that can be read by a custom program.  

Here is a list of recommended software to use with this:  

* OpenTOPAS: https://github.com/OpenTOPAS/OpenTOPAS.git. Follow the QuickStart installation guides for [Debian](https://github.com/OpenTOPAS/OpenTOPAS/blob/master/OpenTOPAS_quickStart_Debian.md) 
             or [MacOS](https://github.com/OpenTOPAS/OpenTOPAS/blob/master/OpenTOPAS_quickStart_forMacOS.md). This extension was developed on machines running at least gcc version 7.5.0 and cmake version 3.17.3
* CASToR: http://www.castor-project.org/ This can be used for image reconstruction of PET images.
* OMEGA: https://github.com/villekf/OMEGA This can be used for image reconstruction of PET images.
* Reconstruction Toolkit (RTK): https://www.openrtk.org/ This can be used for image reconstruction of CT images.
* ImageJ: https://imagej.nih.gov/ij/ This can read the binary output files from OpenTOPAS.

## Elements of a parameter file

A parameter file for an imaging simulation has three main parts: the source, the detector geometry, and the scorer. We have example parameter files for each type of imaging we support with all the parameters for our custom components laid out.

### Scoring

All the scorers are based off of DigitizerScorer, a scorer that works like a phase space except split up so it detects energy despositions within specific scintillator crystals instead of continuously along a surface like a normal phase space. There are scorers available for each supported imaging modality:

* PETScorer for use on a RingImager geometry.
* PGScorer for use on a FlatImager geometry.
* CBCTScorer for use on a FlatImager geometry.

### Geometry

The specifications of the detector geometry can be put into the OpenTOPAS simulation by using either a RingImager or FlatImager geometry component.

### Source

* PET: A standard OpenTOPAS positron source can be used to automatically get annihilation events for detection.
* SPECT: A standard OpenTOPAS photon source will work.
* PG: A standard OpenTOPAS proton source can be used to produce prompt gammas.
* CBCT: There is an example of the source using standard OpenTOPAS in the example parameter file.

More documentation can be found in [OpenTOPAS documentation](https://opentopas.readthedocs.io/en/latest/extensions/imaging/index.html) 
