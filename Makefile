INCLUDE_DIRS = /usr/local/include/opencv2 /usr/local/include/opencv
LINK_DIRS = /usr/local/lib
LINK_LIB = opencv_highgui opencv_core opencv_calib3d opencv_imgproc opencv_features2d curses
CORE = aaskype
CXX_FLAGS = -O2 $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -L, $(LINK_DIRS)) $(addprefix -l, $(LINK_LIB))

.PHONEY: all clean initialize

all: $(CORE)

clean:
	$(RM) $(CORE)

initialize:
	sudo brew install opencv

$(CORE): main.cc
	$(CXX) $< -o $@ $(CXX_FLAGS)

