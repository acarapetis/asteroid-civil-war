# Makefile for asteroids

CXX?=g++
FLAGMODE=
MODE=$(shell if [ ! -z $(FLAGMODE) ]; then echo -$(FLAGMODE); fi)
OPTIMIZATIONS=$(shell if [ -z $(FLAGMODE) ]; then echo -O3; fi)
WARNINGS=-Wall
EXTRA=$(shell if [ ! -z $(FLAGMODE) ]; then echo -g; fi) $(shell if [ profile = "$(FLAGMODE)"  ]; then echo -pg; fi)
DEFINES=$(shell if [ ! -z $(FLAGMODE) ]; then echo -DDEBUGMODE; fi)
CFLAGS = -std=c++20 $(WARNINGS) $(OPTIMIZATIONS) -I/usr/local/include -I/usr/include $(DEFINES)
LIBS = $(OPTIMIZATIONS) -L/usr/local/lib -L/usr/lib -lallegro$(MODE) -lstdc++ -lm -lallegro_ttf$(MODE) -lallegro_font$(MODE) -lallegro_primitives$(MODE) -lallegro_image$(MODE)

OBJECTS = point.o mouse.o keyboard.o camera.o blending.o graphics.o object.o particles.o asteroids.o game.o main.o

all:	$(OBJECTS)
	$(CXX) $(EXTRA) -flto -o asteroids $(OBJECTS) $(LIBS)

-include $(OBJECTS:.o=.d)

$(OBJECTS):	%.o:%.cpp
	$(CXX) $(EXTRA) $(CFLAGS) -flto -c $<
	$(CXX) $(EXTRA) $(CFLAGS) -MM $*.cpp > $*.d

clean:
	@rm -f *.o *.d game
