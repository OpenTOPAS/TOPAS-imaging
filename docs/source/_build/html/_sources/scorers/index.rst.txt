Scorers
=========================================


In this extension, we have 2 main scorers.

1. :ref:`Binned scorer`
2. :ref:`NTuple scorer`

You can also add :ref:`Digitizers` to the scorers to process detected signals. 

.. _Binned scorer:

Binned scorer
*************

The binned scorer accumuates detected signal in divided volume.

The binned scorer is useful for CBCT or Prompt gamma acquisition.

You need to set the number of bins on the scoring volume to score by binned::

    i:Ge/Scorer/XBins     = 512
    i:Ge/Scorer/YBins     = 512

If you are using FlatImager for a detector, there are more than one surface and you need to define which surface will be used for scoring.
To score on the surface between scitillator and photo detector layers on FlatImager, you need to set::

    s:Sc/Scorer/Surface = "Imager/PhotoDetectorScintillatorInterface"



.. _NTuple scorer:

NTUple scorer
*************

The NTuple scorer saves the information of detected signals per each line similar to PhaseSpace.

The NTupble scorer is useful for PET or SPECT image acquisition.


.. _Digitizers:

Digitizers
**********

We have implemnted the following digitizers:

* Time resolution
* Dead time
* Energy cutoff
* Sigmoial threshold
* Pulse pile up

You can build digitizer chain by::

    sv:Sc/PETScore/Digitizers   = 3 "EnergyCutoff" "TimeResolution" "DeadTime"

The digitizers will be applied sequentially as set by user.

Parameters for digitizers can be defined as follow::
    
    d:Sc/PETScore/Digitizer/TimeResolution                 = 500 ps # Time resolution
    d:Sc/PETScore/Digitizer/DeadTime                       = 10. ms # Dead time
    d:Sc/PETScore/Digitizer/EnergyCutoff/Threshold         = 400 keV # Energy cutoff
    d:Sc/PETScore/Digitizer/EnergyCutoff/Uphold            = 650 keV # Energy cutoff
    d:Sc/PETScore/Digitizer/SigmoidalThresholder/Threshold = 650 keV # Energy cutoff
    u:Sc/PETScore/Digitizer/SigmoidalThresholder/Alpha     = 50. 
    u:Sc/PETScore/Digitizer/SigmoidalThresholder/Percent   = 0.9
    d:Sc/PETScore/Digitizer/PulsePileUp/TimingWindow       = 100 ns


.. _Optical spread function:

Optical spread function
***********************

In radiation detection, photon is converted into visible light (a.k.a. optical photon) to be detected.

However, an interaction between photon and scintillating crystal creates tens of thousand optical photons, thus it slows down the simulation.
To improve the computational efficiency of the simulation, optical spread function (`[Shi, et al., Physics in Medicine and Biology (2019)] <https://iopscience.iop.org/article/10.1088/1361-6560/ab12aa>`_ and `[O'Connell and Bazalova-Carter, Medical Physics (2021)] <https://aapm.onlinelibrary.wiley.com/doi/abs/10.1002/mp.15007>`_) was proposed as an variance reduction technique.

The idea of OSF is to numerically model the spread of optical photon in Gaussian function instead of track each optical photon.

We have implemented scorer using optical spread function. To utilize the scorer, one needs to measure optical spread function (OSF) separately and provides them for the simulation::

    #============== Optical spread function parameters ========
    i:Sc/CBCTscorer/HKernelU                       = 5 # size of OSF kernels
    i:Sc/CBCTscorer/HKernelV                       = 5 # size of OSF kernels
    d:Sc/CBCTscorer/InitialEnergy                  = 10.0 keV
    d:Sc/CBCTscorer/EnergyStep                     = 5.0 keV
    dv:Sc/CBCTscorer/DetectorEfficiency/Energies   = 27 10.0 15.0 20.0 25.0 30.0 35.0 40.0 45.0 50.0 55.0 60.0 65.0 70.0 75.0 80.0 85.0 90.0 95.0 100.0 105.0 110.0 115.0 120.0 125.0 130.0 135.0 140.0 keV
    uv:Sc/CBCTscorer/DetectorEfficiency/Efficiency = 27 0.000398 0.026029 0.067879 0.080005 0.069582 0.079304 0.064622 0.069822 0.069503 0.065332 0.059056 0.052028 0.045107 0.038802 0.033204 0.028320 0.024101 0.020590 0.017658 0.015201 0.013128 0.011383 0.009911 0.008660 0.007598 0.006690 0.005907
    s:Sc/CBCTscorer/OSFPath                        = "<path-to-osfs>"
    b:Sc/CBCTscorer/ForceInteraction               = "True"


TOPAS parameters to simulate OSF can be found in the :doc:`example </examples/OSF>`.

One can find detail description of measuring OSF in the previous publication (`Shi, et al., Physics in Medicine and Biology (2019) <https://iopscience.iop.org/article/10.1088/1361-6560/ab12aa>`_)