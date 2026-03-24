CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -Isrc

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Direktori source code
SRC_DIRS = src src/math src/geometry src/core src/graphics
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
# Mengubah ekstensi .cpp menjadi .o untuk object files
OBJS = $(SRCS:.cpp=.o)

# Deteksi Sistem Operasi (Cross-Platform compatibility)
ifeq ($(OS),Windows_NT)
	# Pengaturan untuk Windows
	TARGET = bin/voxelizer.exe
	MKDIR = if not exist bin mkdir bin
	RM = del /S /Q $(subst /,\,$(OBJS)) 2>nul & del /Q bin\voxelizer.exe 2>nul
	RUN_CMD = $(TARGET)
else
	# Pengaturan untuk Linux / MacOS
	TARGET = bin/voxelizer
	MKDIR = mkdir -p bin
	RM = rm -f $(OBJS) $(TARGET)
	RUN_CMD = ./$(TARGET)
endif

.PHONY: all clean run

all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compiling
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Membersihkan file kompilasi
clean:
	-@$(RM)

# Menjalankan program (Hanya untuk testing tanpa argumen)
run: all
	$(RUN_CMD)