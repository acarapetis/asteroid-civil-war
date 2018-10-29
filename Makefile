# Makefile for asteroids

CXX=g++
FLAGMODE=
MODE=$(shell if [ ! -z $(FLAGMODE) ]; then echo -$(FLAGMODE); fi)
OPTIMIZATIONS=$(shell if [ -z $(FLAGMODE) ]; then echo -O3; fi)
WARNINGS=-Wall
EXTRA=$(shell if [ ! -z $(FLAGMODE) ]; then echo -g; fi) $(shell if [ profile = "$(FLAGMODE)"  ]; then echo -pg; fi)
DEFINES=$(shell if [ ! -z $(FLAGMODE) ]; then echo -DDEBUGMODE; fi)
CFLAGS = $(WARNINGS) $(OPTIMIZATIONS) -I/usr/local/include -I/usr/include $(DEFINES)
LIBS = $(OPTIMIZATIONS) -L/usr/local/lib -L/usr/lib -lallegro$(MODE)-4.9.18  -lstdc++ -lallegro_ttf$(MODE)-4.9.18 -lallegro_font$(MODE)-4.9.18 -lallegro_primitives$(MODE)-4.9.18 -lallegro_image$(MODE)-4.9.18

OBJECTS = global.o point.o mouse.o keyboard.o camera.o blending.o graphics.o object.o particles.o asteroids.o game.o main.o

all:	$(OBJECTS)
	$(CXX) $(EXTRA) -o asteroids $(OBJECTS) $(LIBS)

$(OBJECTS):	%.o:%.cpp %.hpp
	$(CXX) $(EXTRA) $(CFLAGS) -c $<

clean:
	@rm -f *.o game
