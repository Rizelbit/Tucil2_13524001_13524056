# IF2211 Tugas Kecil 2 - 3D Object Voxelization using Octree

## 📖 Project Overview
This program is a C++ based Command Line Interface (CLI) application designed to convert standard 3D models (polygon surface representations) with a `.obj` extension into voxel-based 3D models (composed of uniform small cubes).

The conversion process implements a **Divide and Conquer** algorithm utilizing an **Octree** spatial data structure. The 3D space is recursively subdivided into eight smaller octants, and the *Separating Axis Theorem* (SAT) is employed to detect intersections between the Axis-Aligned Bounding Boxes (AABBs) and the triangular faces of the object. Bounding boxes that intersect the model at the user-specified maximum depth are extracted as voxels. Furthermore, the program outputs detailed execution statistics to the CLI, including the total number of nodes formed, pruned nodes per depth level, generated voxels, and the overall execution time.

## 💻 System Requirements
To compile and execute this program, the following requirements must be met:
* **C++ Compiler:** Must support the **C++17** standard (e.g., `g++` or `clang++`).
* **Make:** A build automation tool (GNU Make for Linux/macOS, or MinGW32-make for Windows).
* **Operating System:** The program is designed to be cross-platform and will run on Windows, Linux, and macOS.

## ⚙️ Compilation Instructions
This repository includes a cross-platform `Makefile` to streamline the build process.
1. Open a Terminal (Linux/macOS) or Command Prompt/PowerShell (Windows).
2. Change the current working directory to the root of this repository (`Tucil2_13524001_13524056`).
3. Execute the following build command:
   ```bash
   make
   ```

(Note for Windows users: If the `make` command is not recognized, utilize `mingw32-make`, ensuring MinGW is added to your system's Environment Variables).
4. Upon successful compilation, an executable file named `voxelizer` (Linux/macOS) or `voxelizer.exe` (Windows) will be generated within the `bin/` directory.
5. To clean the build directory and remove compiled objects, execute:
   ```bash
   make clean
   ```

## 🚀 Execution and Usage Guide

The program requires two command-line arguments to run: the file path of the input `.obj` model and the maximum depth for the Octree subdivision.

**Command Format:**
```bash
./bin/voxelizer <path_to_obj_file> <max_depth>
```

**Execution Example (Windows):**
```bash
bin\voxelizer.exe test\pumpkin.obj 4
```

**Execution Example (Linux / macOS):**
```bash
./bin/voxelizer test/pumpkin.obj 4
```

**Expected Output:**
* The system will parse and validate the `v` (vertices) and `f` (faces) definitions from the input file.
* Real-time progression will be displayed, followed by a comprehensive statistical report of the Octree generation.
* A new voxelized 3D model (e.g., `pumpkin-voxelized.obj`) will be generated and saved in the same directory as the input file.

## 👨‍💻 Authors

This project was collaboratively developed by:
| Student Number | Full Name |
| :---: | :--- |
| 13524001 | Samuelson D. Tanuraharja |
| 13524056 | Reinhard Alfonzo Hutabarat |

