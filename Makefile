
CXX=g++-4.8


OBJS=matrix-app.o 
TARGET=matrix-app

CXXFLAGS=-std=c++11
RGB_INCDIR=matrix/include
RGB_LIBDIR=matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread -lopencv_core -lopencv_highgui -lopencv_imgproc -lSDL -lSDL_mixer


all: $(TARGET)

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)


$(TARGET): $(OBJS) $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o : %.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<


install:
	cp $(TARGET) /usr/local/bin/$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
	$(MAKE) -C matrix clean

