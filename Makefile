.PHONY: all main clean debug
NAME=hesp
CXX= g++
LIB= -lOpenCL
CPPFLAGS= -Ofast -Wall --diagnostics-color=always -std=c++0x
SRC= main.cpp

HEADERS= $(shell find . -iname "*.hpp")

all: $(NAME)


run: $(NAME)
	./$(NAME)

debug: $(SRC) $(HEADERS)
	$(CXX) $(CPPFLAGS) -g $(INCLUDE) $(SRC) -o $(NAME) $(LIB)

$(NAME): $(SRC) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(INCLUDE) $(SRC) -o $(NAME) $(LIB)


profile:  $(SRC) $(HEADERS)
	$(CXX) $(CPPFLAGS) -fprofile-generate=./$(NAME).prof $(SRC) -o $(NAME) $(LIB)
	./$(NAME)
	$(CXX) $(CPPFLAGS) -fprofile-use=$(NAME).prof  $(SRC) -o $(NAME) $(LIB)


clean:
	rm -f $(NAME)
