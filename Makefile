
CXX=arm-linux-gnueabihf-g++


OBJS=matrix-app.o 
TARGET=matrix-app


RGB_INCDIR=matrix/include
RGB_LIBDIR=matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread


all: $(TARGET)

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)


$(TARGET): $(OBJS) $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o : %.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<



clean:
	rm -f $(OBJECTS) $(TARGET)
	$(MAKE) -C matrix clean

