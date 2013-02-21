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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/processing/idx_range.o \
	${OBJECTDIR}/src/communication/ConnConfig.o \
	${OBJECTDIR}/src/circular_queue.o \
	${OBJECTDIR}/src/communication/baseutils/netiobuf.o \
	${OBJECTDIR}/src/communication/baseutils/ZTPServer.o \
	${OBJECTDIR}/src/communication/baseutils/TCPServer.o \
	${OBJECTDIR}/src/system_config.o \
	${OBJECTDIR}/src/communication/baseutils/TCPClientNode.o \
	${OBJECTDIR}/src/buffer_function.o \
	${OBJECTDIR}/src/processing/forest_map.o \
	${OBJECTDIR}/src/tinystr.o \
	${OBJECTDIR}/src/dlhandler.o \
	${OBJECTDIR}/src/communication/CEPConnHandler.o \
	${OBJECTDIR}/src/tinyxml.o \
	${OBJECTDIR}/src/communication/baseutils/ZTPClient.o \
	${OBJECTDIR}/src/communication/baseutils/growbuf.o \
	${OBJECTDIR}/src/processing/pe4initest.o \
	${OBJECTDIR}/src/utils_time.o \
	${OBJECTDIR}/src/dynamic_struct.o \
	${OBJECTDIR}/src/tinyxmlparser.o \
	${OBJECTDIR}/src/communication/baseutils/ZTPSendThread.o \
	${OBJECTDIR}/src/tinyxmlerror.o \
	${OBJECTDIR}/src/utils_cstr2nbr.o \
	${OBJECTDIR}/src/config_metadata.o \
	${OBJECTDIR}/src/event2text.o \
	${OBJECTDIR}/src/processing/plugin_function.o \
	${OBJECTDIR}/src/communication/baseutils/ZTPServerHandler.o \
	${OBJECTDIR}/src/processing/idx_trie_tree.o \
	${OBJECTDIR}/src/utils_xml.o \
	${OBJECTDIR}/src/communication/baseutils/mmap.o \
	${OBJECTDIR}/src/communication/baseutils/TCPSocket.o \
	${OBJECTDIR}/src/processing/trie_map_tree.o \
	${OBJECTDIR}/src/processing/router.o \
	${OBJECTDIR}/src/const_dataset.o \
	${OBJECTDIR}/src/communication/CEPDispatch.o \
	${OBJECTDIR}/src/buffer_metadata.o \
	${OBJECTDIR}/src/processing/pn.o \
	${OBJECTDIR}/src/processing/idx_rbtree.o \
	${OBJECTDIR}/src/utils_nbr2str.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib -L/usr/lib64/ -Llib -llzo2

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcepframe.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcepframe.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -Wl,-G,-brtl,-bdynamic -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcepframe.${CND_DLIB_EXT} -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/processing/idx_range.o: src/processing/idx_range.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/idx_range.o src/processing/idx_range.cpp

${OBJECTDIR}/src/communication/ConnConfig.o: src/communication/ConnConfig.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/ConnConfig.o src/communication/ConnConfig.cpp

${OBJECTDIR}/src/circular_queue.o: src/circular_queue.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/circular_queue.o src/circular_queue.cpp

${OBJECTDIR}/src/communication/baseutils/netiobuf.o: src/communication/baseutils/netiobuf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/netiobuf.o src/communication/baseutils/netiobuf.cpp

${OBJECTDIR}/src/communication/baseutils/ZTPServer.o: src/communication/baseutils/ZTPServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/ZTPServer.o src/communication/baseutils/ZTPServer.cpp

${OBJECTDIR}/src/communication/baseutils/TCPServer.o: src/communication/baseutils/TCPServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/TCPServer.o src/communication/baseutils/TCPServer.cpp

${OBJECTDIR}/src/system_config.o: src/system_config.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/system_config.o src/system_config.cpp

${OBJECTDIR}/src/communication/baseutils/TCPClientNode.o: src/communication/baseutils/TCPClientNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/TCPClientNode.o src/communication/baseutils/TCPClientNode.cpp

${OBJECTDIR}/src/buffer_function.o: src/buffer_function.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/buffer_function.o src/buffer_function.cpp

${OBJECTDIR}/src/processing/forest_map.o: src/processing/forest_map.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/forest_map.o src/processing/forest_map.cpp

${OBJECTDIR}/src/tinystr.o: src/tinystr.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tinystr.o src/tinystr.cpp

${OBJECTDIR}/src/dlhandler.o: src/dlhandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/dlhandler.o src/dlhandler.cpp

${OBJECTDIR}/src/communication/CEPConnHandler.o: src/communication/CEPConnHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/CEPConnHandler.o src/communication/CEPConnHandler.cpp

${OBJECTDIR}/src/tinyxml.o: src/tinyxml.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tinyxml.o src/tinyxml.cpp

${OBJECTDIR}/src/communication/baseutils/ZTPClient.o: src/communication/baseutils/ZTPClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/ZTPClient.o src/communication/baseutils/ZTPClient.cpp

${OBJECTDIR}/src/communication/baseutils/growbuf.o: src/communication/baseutils/growbuf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/growbuf.o src/communication/baseutils/growbuf.cpp

${OBJECTDIR}/src/processing/pe4initest.o: src/processing/pe4initest.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/pe4initest.o src/processing/pe4initest.cpp

${OBJECTDIR}/src/utils_time.o: src/utils_time.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/utils_time.o src/utils_time.cpp

${OBJECTDIR}/src/dynamic_struct.o: src/dynamic_struct.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/dynamic_struct.o src/dynamic_struct.cpp

${OBJECTDIR}/src/tinyxmlparser.o: src/tinyxmlparser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tinyxmlparser.o src/tinyxmlparser.cpp

${OBJECTDIR}/src/communication/baseutils/ZTPSendThread.o: src/communication/baseutils/ZTPSendThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/ZTPSendThread.o src/communication/baseutils/ZTPSendThread.cpp

${OBJECTDIR}/src/tinyxmlerror.o: src/tinyxmlerror.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tinyxmlerror.o src/tinyxmlerror.cpp

${OBJECTDIR}/src/utils_cstr2nbr.o: src/utils_cstr2nbr.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/utils_cstr2nbr.o src/utils_cstr2nbr.cpp

${OBJECTDIR}/src/config_metadata.o: src/config_metadata.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/config_metadata.o src/config_metadata.cpp

${OBJECTDIR}/src/event2text.o: src/event2text.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/event2text.o src/event2text.cpp

${OBJECTDIR}/src/processing/plugin_function.o: src/processing/plugin_function.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/plugin_function.o src/processing/plugin_function.cpp

${OBJECTDIR}/src/communication/baseutils/ZTPServerHandler.o: src/communication/baseutils/ZTPServerHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/ZTPServerHandler.o src/communication/baseutils/ZTPServerHandler.cpp

${OBJECTDIR}/src/processing/idx_trie_tree.o: src/processing/idx_trie_tree.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/idx_trie_tree.o src/processing/idx_trie_tree.cpp

${OBJECTDIR}/src/utils_xml.o: src/utils_xml.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/utils_xml.o src/utils_xml.cpp

${OBJECTDIR}/src/communication/baseutils/mmap.o: src/communication/baseutils/mmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/mmap.o src/communication/baseutils/mmap.cpp

${OBJECTDIR}/src/communication/baseutils/TCPSocket.o: src/communication/baseutils/TCPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication/baseutils
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/baseutils/TCPSocket.o src/communication/baseutils/TCPSocket.cpp

${OBJECTDIR}/src/processing/trie_map_tree.o: src/processing/trie_map_tree.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/trie_map_tree.o src/processing/trie_map_tree.cpp

${OBJECTDIR}/src/processing/router.o: src/processing/router.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/router.o src/processing/router.cpp

${OBJECTDIR}/src/const_dataset.o: src/const_dataset.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/const_dataset.o src/const_dataset.cpp

${OBJECTDIR}/src/communication/CEPDispatch.o: src/communication/CEPDispatch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/communication
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/communication/CEPDispatch.o src/communication/CEPDispatch.cpp

${OBJECTDIR}/src/buffer_metadata.o: src/buffer_metadata.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/buffer_metadata.o src/buffer_metadata.cpp

${OBJECTDIR}/src/processing/pn.o: src/processing/pn.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/pn.o src/processing/pn.cpp

${OBJECTDIR}/src/processing/idx_rbtree.o: src/processing/idx_rbtree.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/processing
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/processing/idx_rbtree.o src/processing/idx_rbtree.cpp

${OBJECTDIR}/src/utils_nbr2str.o: src/utils_nbr2str.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -DMLOG_LEVEL=0 -D__FLAG4INHERITABLE__cancel -D__VER4QUEUE_BLOCKING_SLEEP__cancel -D__VER4QUEUE_MT_WRAP__cancle -D__VER4TEST_TEMPLATE_SPECIALIZATION__cancel -D__WORDSIZE=64 -Iinclude -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/utils_nbr2str.o src/utils_nbr2str.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcepframe.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
