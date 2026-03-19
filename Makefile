CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -Isrc
LDFLAGS = -lmingw32 -lSDL2main -lSDL2

# Direktori source code
SRC_DIRS = src src/math src/geometry src/core src/renderer
# Mencari semua file .cpp di dalam direktori-direktori tersebut
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
# Mengubah ekstensi .cpp menjadi .o untuk object files
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = bin/voxelizer.exe

.PHONY: all clean run

all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compiling
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Membersihkan file kompilasi
clean:
	del /S /Q src\*.o
	del /Q bin\*.exe

# Menjalankan program
run: all
	$(TARGET)
