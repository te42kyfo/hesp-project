.PHONY: hesp-gl all main clean debug

CXX= g++

HESP_LIB= -lOpenCL
HESP_CPPFLAGS= -Ofast -Wall -std=c++0x

HESP_GL_LIB= -lGL -lGLEW `sdl2-config --libs` -lOpenCL
HESP_GL_CPPFLAGS= -Ofast -Wall -std=c++0x `sdl2-config --cflags`

HESP_SRC= main.cpp parfile_reader.cpp simulation.cpp ocl.cpp
HESP_GL_SRC= hesp_gl.cpp ./vis/sdl_gl.cpp ./vis/draw_particles.cpp parfile_reader.cpp simulation.cpp ocl.cpp

HEADERS= $(shell find . -iname "*.hpp")

all: hesp

hesp: $(HESP_SRC) $(HEADERS)
	$(CXX) $(HESP_CPPFLAGS) $(INCLUDE) $(HESP_SRC) -o $@ $(HESP_LIB)


hesp_gl: $(HESP_GL_SRC) $(HEADERS)
	$(CXX) $(HESP_GL_CPPFLAGS) $(HESP_GL_SRC)   $(INCLUDE) -o $@ $(HESP_GL_LIB) 


clean:
	@rm -f hesp
	@rm -f hesp_gl
