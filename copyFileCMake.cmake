# copy_py_files.cmake
cmake_minimum_required(VERSION 3.8)
# Source and destination directories as arguments
set(DEST_ROOT "${DEST_ROOT}")
set(SOURCE_DIR "${SOURCE_DIR}")

set(SOURCE_PY_DIR ${SOURCE_DIR})
set(SOURCE_MTD_DIR ${SOURCE_DIR})
set(SOURCE_DLL_DIR ${SOURCE_DIR}/Debug)

# All PY files goes to this dir
set(DEST_PY_DIR ${DEST_ROOT}/scripts/mtoa/ui/ae)

# Both DLL and MTD files goes to this dir
set(DEST_DLL_DIR ${DEST_ROOT}/shaders)


# Copy only .py files
file(GLOB PY_FILES "${SOURCE_PY_DIR}/*.py")
foreach(PY_FILE IN LISTS PY_FILES)
    file(COPY ${PY_FILE} DESTINATION ${DEST_PY_DIR})
endforeach()


# Copy only .mtd files
file(GLOB MTD_FILES "${SOURCE_MTD_DIR}/*.mtd")
foreach(MTD_FILE IN LISTS MTD_FILES)
    file(COPY ${MTD_FILE} DESTINATION ${DEST_DLL_DIR})
endforeach()


# Copy only .dll files
file(GLOB DLL_FILES "${SOURCE_DLL_DIR}/*.dll")
foreach(DLL_FILE IN LISTS DLL_FILES)
    file(COPY ${DLL_FILE} DESTINATION ${DEST_DLL_DIR})
endforeach()