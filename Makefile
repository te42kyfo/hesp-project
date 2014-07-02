.PHONY: hesp-gl all main clean debug

CXX= g++

HESP_LIB= -lOpenCL
HESP_CPPFLAGS= -Ofast -Wall -std=c++0x -g

HESP_GL_LIB= -lGL -lGLEW `sdl2-config --libs` -lOpenCL
HESP_GL_CPPFLAGS= -Ofast -Wall -std=c++0x `sdl2-config --cflags` -g

HESP_SRC= main.cpp parfile_reader.cpp simulation.cpp ocl.cpp 
HESP_GL_OBJ= hesp_gl.o ./vis/sdl_gl.o ./vis/draw_particles.o parfile_reader.o simulation.o ocl.o vis/lodepng.o dtime.o

HEADERS= $(shell find . -iname "*.hpp")

all: hesp

hesp: $(HESP_SRC) $(HEADERS)
	$(CXX) $(HESP_CPPFLAGS) $(INCLUDE) $(HESP_SRC) -o $@ $(HESP_LIB)


hesp_gl: $(HESP_GL_OBJ)
	$(CXX) -o $@  $^ $(HESP_GL_LIB)

%.o : %.cpp $(HEADERS)
	$(CXX) $(HESP_GL_CPPFLAGS) -c -o $@ $<


clean:
	@rm -f *.o
	@rm -f hesp
	@rm -f hesp_gl
