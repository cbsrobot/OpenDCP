#--set OS Specific Compiler Flags-----------------------------------------------
IF(TARGET_ARCH STREQUAL "x86_64")
    SET(AUTO_CFLAGS "CFLAGS=-fPIC -O3")
ELSE()
    SET(AUTO_CFLAGS "CFLAGS=-m32")
    SET(TARGET --target=i686)
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
ENDIF()
#-------------------------------------------------------------------------------

#--set os specifc linking mode-------------------------------------------------
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules/)

FIND_LIBRARY(ZLIB REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
SET(LIBS ${LIBS} ${ZLIB_LIBRARIES})

FIND_PACKAGE(OpenSSL REQUIRED)
INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
SET(LIBS ${LIBS} ${OPENSSL_LIBRARIES})

FIND_PACKAGE(LibXml2 REQUIRED)
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

IF(ENABLE_XMLSEC)
    FIND_PACKAGE(LibXslt REQUIRED)
    INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})
    SET(LIBS ${LIBS} ${LIBXSLT_LIBRARIES})

    IF(XMLSEC_DOWNLOAD)
        INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/contrib/include/libxslt)
        INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/contrib/include/xmlsec1)
        SET(COMPILE_XMLSEC 1)
        SET(LIBS ${LIBS} ${PREFIX}/lib/libxmlsec1-openssl.a ${PREFIX}/lib/libxmlsec1.a)
    ELSE()
        FIND_PACKAGE(XMLSec)
        INCLUDE_DIRECTORIES(${XMLSEC1_INCLUDE_DIR})
        INCLUDE_DIRECTORIES(${XMLSEC1-OPENSSL_INCLUDE_DIR})
        SET(LIBS ${LIBS} ${XMLSEC1_LIBRARIES})
        SET(LIBS ${LIBS} ${XMLSEC1-OPENSSL_LIBRARIES})
    ENDIF()
ENDIF(ENABLE_XMLSEC)

FIND_PACKAGE(EXPAT REQUIRED)
INCLUDE_DIRECTORIES(${XMLSEC1_INCLUDE_DIR})
SET(LIBS ${LIBS} ${EXPAT_LIBRARIES})

FIND_PACKAGE(TIFF REQUIRED)
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
SET(LIBS ${LIBS} ${TIFF_LIBRARIES})

SET(COMPILE_OPENJPEG 1)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/contrib/include)
SET(LIBS ${LIBS} ${PREFIX}/lib/libopenjpeg.a)
#-------------------------------------------------------------------------------