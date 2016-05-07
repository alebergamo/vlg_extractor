                      VLG_EXTRACTOR

                         Alessandro Bergamo
                              Chen Fang
                          Lorenzo Torresani
                     {aleb,chen,lorenzo}@cs.dartmouth.edu

SUMMARY:
  Given an input image, this software extracts the:
  - Classeme descriptor  [1]  (continuous and binary version)
  - PiCoDes descriptor   [2]  (128, 1024 and 2048 bits)
  - MetaClass descriptor [3]  (mc and mc-bit)

COPYRIGHT WARNING:
  This software can be used for research and evaluation purposes only.
  Commercial use is prohibited.
  Public redistribution of the code or its derivatives is prohibited.
  See the included file LICENSE.txt for more information.
  For the SIFT extraction, this software exploits a third-party software [4],
  copyrighted by the UCLA VisionLab. See the included license
  file (LICENSE_UCLA.txt) for more information.
  For the SSIM extraction, this software exploits a third-party software [5,6],
  copyrighted by the University of Augsburg. See the included license
  file (LICENSE_UNI_AUGSBURG.txt) for more information.

INSTALLATION and USAGE:
  1) Edit the Makefile according to your specific environment.
     Set the C++ compiler path, and the OpenCV include and library path.
     If you use gcc, make sure to link against libstdc++ instead of libc++
  2) Compile the software invoking "make".
     The compilation will create the command-line tool ./vlg_extractor
  3) Download the parameter files using the script
     data/download_parameters.sh
     In case you will need to move the vlg_extractor
     executable or the "data" to a different location, please
     specify the new location of the data folder using the option
     "--parameters-dir" when invoking vlg_extractor

NOTES:
  * The software supports the following image formats:
    + Windows bitmaps - BMP, DIB
    + JPEG files - JPEG, JPG, JPE
    + Portable Network Graphics - PNG
    + Portable image format - PBM, PGM, PPM
    + Sun rasters - SR, RAS
    + TIFF files - TIFF, TIF

  * For the mc-bit descriptor, we also release the list of the features resulted
    from the Recursive Feature Elimination analysis made in Fig. 3 of [3].
    This list can be used to have a lower-dimensional version of the mc-bit descriptor.
    Please refer to the file "idx_features_RFE_on_ILSVRC2010_mc_bit.mat".
    The indices of the features are ordered for increasing importance,
    i.e. the top-F features are idx_features_RFE_on_ILSVRC2010_mc_bit(1:F).

EXAMPLES:
  * Let's suppose the directory /data/Caltech contains all our
    images, divided by category (e.g. /data/Caltech/001.ak_47/,
    /data/Caltech/002.American_flag/, etc.) and we want to save the classeme
    vectors in the directory /features, keeping the same directory structure
    (i.e. /features/001.ak_47, etc..).
    First we need to create the list of the image files.
    (e.g. under the assumption that every image has extension ".jpg", this can
	be done with the following Unix command:
    cd /data/ && find ./ -iname "*.jpg" > /tmp/listimages.txt"      ).
    Now, the text file contains "001.ak_47/0001.jpg, 001.ak_47/0002.jpg, etc.."
    and we can extract the classemes to a FLOAT-type file
    with the following command:
     > ./vlg_extractor --extract_classemes=FLOAT --parameters-dir=<your parameter dir>
                       /tmp/listimages.txt /data/Caltech /features


REFERENCES:
  [1] Efficient Object Category Recognition using Classemes,
      Lorenzo Torresani et others, ECCV2010
  [2] Alessandro Bergamo, Lorenzo Torresani, Andrew Fitzgibbon
      PiCoDes: Learning a Compact Code for Novel-Category Recognition, NIPS 2011
  [3] Alessandro Bergamo, Lorenzo Torresani
      Meta-Class Features for Large-Scale Object Categorization on a Budget
  [4] A LIGHTWEIGHT C++ IMPLEMENTATION OF DAVID LOWE'S SCALE INVARIANT
      FEATURE TRANSFORMS, Andrea Vedaldi, http://www.cs.ucla.edu/~vedaldi
  [5] Matching local self-similarities across images and videos.
      Shechtman, E., Irani, M., CVPR 2007
  [6] Comparing Local Feature Descriptors in pLSA-Based Image Models.
      Eva Horster, Thomas Greif, Rainer Lienhart, Malcolm Slaney.
      30th Annual Symposium of the German Association for
      Pattern Recognition (DAGM) 2008, Munich, Germany, June 2008.


