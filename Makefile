CORE = aaskype

.PHONEY: all clean initialize

all: $(CORE)

clean:
	$(RM) $(CORE)

initialize:
	sudo brew install opencv


CXXFLAGS = \
		   -I/usr/local/include

LDFLAGS = \
		  -L/usr/local/lib \
		  -lopencv_core \
		  -lopencv_highgui \
		  -lopencv_features2d \
		  -lopencv_imgproc \
		  -lopencv_nonfree \
		 -lcurses

$(CORE): main.cc
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
