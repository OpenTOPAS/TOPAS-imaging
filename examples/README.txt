This directory contains 3 examples of TOPAS-imaging extension

1. CBCTc
- Main.txt, SimpleCBCT.txt, and SimpleCBCT_PCD.txt build a flat-panel detector, the Shepp-Logan phantom, and an X-ray tube.
- Main.txt uses CBCTScorer, which scores optical photons.
- Optical Spread Function can be enabled for variance reduction.
- OSF.tar.gz should be unzipped to run it.

- SimpleCBCT.txt and SimpleCBCT_PCD.txt score energy deposition of photon on the flat-panel detector.

- SimpleCBCT_PCD.txt models a photon-counting detector using the energy binning feature of OpenTOPAS.

- The examples will save projection data of the Shepp-Logan phantom.

2. OSF
- This example builds a geometry, scorers, optical materials, and a detector to obtain OSF.


3. PET
- This example builts the ACR-type phantom, a ring-shaped detector.
- Main.txt and PETSystem_example.txt build a CylindricalPET system.
- NeuroPETSystem.txt build a NeuroPET system.
- The example saves a phase space file (.phsp and .header) containing a list of coincident photons.

