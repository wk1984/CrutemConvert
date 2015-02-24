#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_DLIB_EXT=dll
CND_CONF=Help_Test
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/CoordCell.o \
	${OBJECTDIR}/src/Date.o \
	${OBJECTDIR}/src/EarthCoordSystem.o \
	${OBJECTDIR}/src/MathUtils.o \
	${OBJECTDIR}/src/ParseArgs.o \
	${OBJECTDIR}/src/Rect.o \
	${OBJECTDIR}/src/StationListFormat.o \
	${OBJECTDIR}/src/StdTypedefs.o \
	${OBJECTDIR}/src/Temperature.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crutemconvert.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crutemconvert.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crutemconvert ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/CoordCell.o: nbproject/Makefile-${CND_CONF}.mk src/CoordCell.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CoordCell.o src/CoordCell.cpp

${OBJECTDIR}/src/Date.o: nbproject/Makefile-${CND_CONF}.mk src/Date.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Date.o src/Date.cpp

${OBJECTDIR}/src/EarthCoordSystem.o: nbproject/Makefile-${CND_CONF}.mk src/EarthCoordSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/EarthCoordSystem.o src/EarthCoordSystem.cpp

${OBJECTDIR}/src/MathUtils.o: nbproject/Makefile-${CND_CONF}.mk src/MathUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MathUtils.o src/MathUtils.cpp

${OBJECTDIR}/src/ParseArgs.o: nbproject/Makefile-${CND_CONF}.mk src/ParseArgs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ParseArgs.o src/ParseArgs.cpp

${OBJECTDIR}/src/Rect.o: nbproject/Makefile-${CND_CONF}.mk src/Rect.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Rect.o src/Rect.cpp

${OBJECTDIR}/src/StationListFormat.o: nbproject/Makefile-${CND_CONF}.mk src/StationListFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/StationListFormat.o src/StationListFormat.cpp

${OBJECTDIR}/src/StdTypedefs.o: nbproject/Makefile-${CND_CONF}.mk src/StdTypedefs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/StdTypedefs.o src/StdTypedefs.cpp

${OBJECTDIR}/src/Temperature.o: nbproject/Makefile-${CND_CONF}.mk src/Temperature.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Temperature.o src/Temperature.cpp

${OBJECTDIR}/src/main.o: nbproject/Makefile-${CND_CONF}.mk src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crutemconvert.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
