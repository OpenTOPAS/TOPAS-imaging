This directory contains 3 examples of TOPAS-imaging extension

1. CBCTc
- Main.txt, SimpleCBCT.txt, and SimpleCBCT_PCD.txt build a flat-panel detector, the Shepp-Logan phantom, and an X-ray tube.

- Main.txt uses CBCTScorer, which scores optical photons.
- Optical Spread Function can be enabled for variance reduction.
- OSF.tar.gz must be unzipped to enable the varince reduction technique.
- The ./CBCT_output/ directory must be created by the user using the mkdir command

- SimpleCBCT.txt and SimpleCBCT_PCD.txt score energy deposition of photon on the flat-panel detector.
- SimpleCBCT_PCD.txt models a photon-counting detector using the energy binning feature of OpenTOPAS.

- Examples will save projection data of the Shepp-Logan phantom.

2. OSF
- This example builds a geometry, scorers, optical materials, and a detector to obtain OSF.
- The example will save OSF of mono-energetic X-ray.
- The ./OSF_CBCT_results/ directory must be created by the user using the mkdir command

3. PET
- This example builts the ACR-type phantom, a ring-shaped detector.
- Main.txt and PETSystem_example.txt build a CylindricalPET system.
- NeuroPETSystem.txt build a NeuroPET system.
- Examples will save phase space files (.phsp and .header) containing a list of coincident photons.

