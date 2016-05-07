CC = gcc
CXX = g++
INCFLAGS += -I/usr/local/opencv-2.4.12/include/
INCFLAGS += -I/usr/local/opencv-2.4.12/include/opencv
LDFLAGS += -L/usr/local/opencv-2.4.12/build/lib

CFLAGS = -Wall -Wno-unused-function -Isrc -Isrc/sift
CFLAGS += -DDEBUG0 -DDEBUG1 -DINCLUDE_EXTRA_FEATURES
CFLAGS += -g -O3

LIBS = -lstdc++ -lm
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann

OBJECTS += src/vlg_toolbox/utils.o src/vlg_toolbox/matrix.o src/vlg_toolbox/list_string.o src/vlg_toolbox/homogeneous_kernel_map.o src/vlg_toolbox/bitmap.o src/vlg_toolbox/opencv.o
OBJECTS += src/gist/extract_gist.o src/gist/gistFeature.o src/gist/gistGabor.o src/gist/makeGabor.o src/gist/makePrefilter.o src/gist/padarraySym.o src/gist/prefilt.o
OBJECTS += src/phog/phog_fc.o
OBJECTS += src/ssim/hist_ssim.o src/ssim/quantize.o src/ssim/SelfSimDescriptor_fc.o
OBJECTS += src/sift/sift.o src/sift/sift-driver.o src/sift/siftpp_wrapper.o src/sift/sift2.o src/sift/sift_center.o
OBJECTS += src/vlg_extractor/vlg_extractor.o src/vlg_extractor/img_features.o src/vlg_extractor/extract_low_level_features.o src/vlg_extractor/extract_classemes.o src/vlg_extractor/extract_picodes.o src/vlg_extractor/extract_metaclass.o


all: vlg_extractor

vlg_extractor: help $(OBJECTS)
	$(CXX) -o vlg_extractor $(OBJECTS) $(LDFLAGS) $(LIBS)

help:
	xxd -i HELP.txt src/vlg_extractor/vlg_extractor_help.h

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

.cpp.o :
	$(CXX) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

clean:
	rm -f vlg_extractor
	find . -iname "*.o" | xargs rm -f

.PHONY: all
.PHONY: count
.PHONY: clean
