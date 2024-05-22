# SPECT | PET | Prompt Gamma | CBCT 

Extentions used to implement SPECT, PET, Prompt Gamma, and CBCT in OpenTOPAS.

# Getting Started

## Tools required

This code is an extension for OpenTOPAS for simulating imaging systems that gives output that can be read by either a custom program or CASToR for image reconstruction.  

Here is a list of recommended software to use with this:  

* OpenTOPAS: https://github.com/OpenTOPAS/OpenTOPAS.git. Follow the QuickStart installation guides for [Debian](https://github.com/OpenTOPAS/OpenTOPAS/blob/master/OpenTOPAS_quickStart_Debian.md) 
             or [MacOS](https://github.com/OpenTOPAS/OpenTOPAS/blob/master/OpenTOPAS_quickStart_forMacOS.md). This extension was developed on machines running at least gcc version 7.5.0 and cmake version 3.17.3
* CASToR: http://www.castor-project.org/ This can be used for image reconstruction of CASToR specific output from this extension.
* ImageJ: https://imagej.nih.gov/ij/ This can read the output files from CASToR image reconstruction with this extension: https://web.archive.org/web/20180213061550/http://www.med.harvard.edu/JPNM/ij/plugins/Interfile.html and its "Plugins/NucMed/Nucmed Open" option.
* AMIDE: This can also read the output files from CASToR image reconstruction

## Elements of a parameter file

A parameter file for an imaging simulation has three main parts: the source, the detector geometry, and the scorer. We have example parameter files for each type of imaging we support with all the parameters for our custom components laid out.

### Scoring

All the scorers are based off of DigitizerScorer, a scorer that works like a phase space except split up so it detects energy despositions within specific scintillator crystals instead of continuously along a surface like a normal phase space. There are scorers available for each supported imaging modality:

* PETScorer for use on a RingDetector geometry.
* SPECTScorer for use on a RingDetector geometry rotated by a time feature. (currently only supports a SPECT system with one head)
* PGScorer for use on a RingDetector geometry. (works the same as a SPECTScorer)
* CBCT Scorer (Work in progress)

### Geometry

The specifications of the detector geometry can be put into the OpenTOPAS simulation by using a RingDetector geometry component. A RingDetector is made up of a given number of detector blocks filled in with a given number of crystals and possibly a collimator, if collimators are used for the chosen ```DetectorType```.

### Source

* PET: A standard OpenTOPAS positron source can be used to automatically get annihilation events for detection or the custom ```ParallelPhotonBeamSource``` can emit parallel photons just like a positron annihilation creates with given error levels from a point. There is currently no volumetric version of this source.
* SPECT: A standard OpenTOPAS photon source will work.
* PG: [TODO]
* CBCT: There is an example of the source using standard OpenTOPAS in the example parameter file.

## Getting your first image

Build the extension and run the example parameter file for your desired imaging modality. You can turn on graphics by setting the ```Active``` parameter for the view to ```"True"```.

For every scanner, CASToR has a .geom file that describes the geometry of the scanner. To use it for reconstruction, it must be in the ```/config/scanner/``` directory of your CASToR installation. You can have the extension automatically put it there by setting the ```CastorGeomDirectory``` to the location of that folder in your file system. Here is an example for somebody named ```username``` who put their CASToR installation in their home directory:  

```s:Ge/Detector/CastorGeomDirectory = "/home/username/CASToR/castor_v3.0.1/config/scanner/"```

You can also move the .geom file to this directory manually after outputting it alongside the other files by setting ```s:Ge/Detector/CastorGeomDirectory = ""```

After it runs, there will be four output files with four extensions: .header, .phsp, .cdh, and .cdf . The .header and .phsp are OpenTOPAS output files that contain raw data from the DigitizerScorer about the readings from the simulation that can be read by external programs (see python scripts for examples). The .cdh file is a CASToR header that describes the measurements for CASToR reconstruction. The .cdf file contains binary data that can be read by CASToR. There may also be the .geom file that needs to be moved to the ```/config/scanner``` directory.

Then you can run this command to reconstruct the image:

```castor-recon -df simulated_scanner.cdh -opti MLEM -it 1:16 -proj joseph -conv gaussian,4.,4.5,3.5::psf -dout output_images```

If you get an error saying the command ```castor-recon``` was not found, you can add the bin directory of your CASToR installation to the PATH or replace ```castor-recon``` with ```[directory of CASToR installation]/bin/castor-recon```.

If it finishes without errors, there will be a new directory called ```output_images``` containing the CASToR output. Each image file is made of an interfile header (.hdr) and a binary file containing image data (.img).

Open ImageJ with the NucMed extension installed and do plugins/NucMed/NucMed Open and select ```output_images/output_images_it1.hdr```. If everything is working, an image should appear with a slider at the bottom that can be used to scroll through layers of the reconstructed image.

More details about the options during CASToR reconstruction can be found in the CASToR docs and by doing ```castor-recon --help```.

Congratulations! You now have an image!

More documentation can be found in [OpenTOPAS documentation](https://opentopas.readthedocs.io/en/latest/extensions/imaging/index.html) 
