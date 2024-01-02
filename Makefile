VPATH	=	src/
CXX	=	g++
CXX_FLAGS	=	-Isrc/
SOURCE	=	main.cpp	src/core.cpp	src/instructions.cpp
LINKER_FLAGS	=	-lSDL2
EXECUTABLE_NAME	=	emulator
all	:	${SOURCE}
	${CXX}	${SOURCE}	${CXX_FLAGS}	${LINKER_FLAGS}	-o	${EXECUTABLE_NAME}