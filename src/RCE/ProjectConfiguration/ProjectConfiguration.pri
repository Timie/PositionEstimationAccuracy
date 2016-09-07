# for custom CONFIGS, always prepend RCE_


# add some important defines...
# set custom debug/warning/critical message format (see DDebug.h)
# DEFINES += DFS_CUSTOM_DEBUG_FORMAT='\\\"[%{if-category}%{category}: %{endif}%{time yyyyMMdd h:mm:ss.zzz}] %{file}:%{line} - %{message}\\\"'
#mirrors debug output to debug file
# DEFINES += DFS_COPY_DEBUG_OUTPUT_TO_FILE='\\\"debug.log\\\"'



#detect compiler suite and target
win32{
  message("Building for platfrom Win32")
  CONFIG += RCE_WIN
}

win64{
  message("Building for platfrom Win64")
  CONFIG += RCE_WIN
}

windows{
  message("Building for platfrom Windows")
  CONFIG += RCE_WIN
}

RCE_WIN{

  DEFINES += RCE_TARGET_WINDOWS

  *-g++* { # MinGW
    message("Building with mingw compiler.")
    CONFIG += RCE_MINGW
  }
  *-msvc* { # MSVC
    message("Building with msvc compiler.")
    CONFIG += RCE_MSVC


      *-msvc2010 { # MSVC
        message("Building with msvc2010 compiler.")
        CONFIG += RCE_MSVC_2010
      }

      *-msvc2012 { # MSVC
        message("Building with msvc2012 compiler.")
        CONFIG += RCE_MSVC_2012
      }

      *-msvc2013 { # MSVC
        message("Building with msvc2013 compiler.")
        CONFIG += RCE_MSVC_2013
      }

  }


  contains(QMAKE_TARGET.arch, x86) {
    message("Compiling for x86")
    DEFINES += RCE_TARGET_ARCH_x86
    CONFIG += RCE_TARGET_ARCH_x86
  }
  contains(QMAKE_TARGET.arch, x86_64) {
    message("Compiling for x64")
    DEFINES += RCE_TARGET_ARCH_x64
    CONFIG += RCE_TARGET_ARCH_x64
  }
}


# FIXME: Add identification of gcc under linux and mac:
# http://stackoverflow.com/questions/11727855/obtaining-current-gcc-architecture
# http://forum.qt.io/topic/18387/qmake-check-for-environment-variables/7


RCE_INTROSPECTION_BUILD { #introspection build - used for internal purposes
    #enables debug output to file with custom message format
    RCE_MSVC {
        # to see, how form proper arguments, see http://www.daviddeley.com/autohotkey/parameters/parameters.htm and https://msdn.microsoft.com/en-us/library/17w5ykft.aspx
#        DEFINES += DFS_CUSTOM_DEBUG_FORMAT='"\\\"[%%%{if-category}%%%{category}: %%%{endif}%%%{time yyyyMMdd h:mm:ss.zzz}] %%%{file}:%%%{line} in %%%{function} - %%%{message}\\\""'
#        DEFINES += DFS_COPY_DEBUG_OUTPUT_TO_FILE='"\\\"debug.log\\\""'
#        DEFINES += QT_MESSAGELOGCONTEXT
#        # turn on all debug outputs
#        DEFINES -= RCE_NO_INFO_OUTPUT RCE_NO_DEBUG_OUTPUT
#        DEFINES += RCE_ENABLE_DEBUG_SCOPES

#        QMAKE_CXXFLAGS += "/DEBUG"
#        QMAKE_LFLAGS += "/INCREMENTAL:NO"

        QMAKE_CXXFLAGS += "/Zi"
        QMAKE_LFLAGS += "/DEBUG"

    }
}

#global debug/release settings (independent of machine or compiler suite
CONFIG(release, debug|release) {
    #in release
    RCE_RETAIL_BUILD { #retail - shipping to a customer
        CONFIG += RCE_USE_OPENCV_WORLD
        #cancels any info or debug output (called by dInfo or dDebug)
        DEFINES += RCE_NO_INFO_OUTPUT RCE_NO_DEBUG_OUTPUT #see DDebug
    }

} else {
    #in debug
}



# defines for compilers

RCE_MSVC{

DEFINES += _CRT_SECURE_NO_WARNINGS

RCE_MSVC_2010{
QMAKE_CXXFLAGS += /we4061 # treat missing enum values in switches as errors
QMAKE_CXXFLAGS += /we4062 # treat missing enum values in switches as errors
}

QMAKE_CXXFLAGS += /we4715 # treat missing returns in non-void functions/methods as errors
QMAKE_CXXFLAGS += /we4150 # treat "deletion of pointer to incomplete type" as error
QMAKE_CXXFLAGS += /wd4265 # class has virtual functions, but destructor is not virtual
QMAKE_CXXFLAGS += /wd4302 # truncation from 'type 1' to 'type 2'
QMAKE_CXXFLAGS += /we4545 # expression before comma evaluates to a function which is missing an argument list
QMAKE_CXXFLAGS += /we4547 # operator before comma has no effect; expected operator with side-effect
QMAKE_CXXFLAGS += /wd4191 # unsafe conversion from 'type of expression' to 'type required'
QMAKE_CXXFLAGS += /we4928 # more than one user-defined conversion has been implicitly applied
QMAKE_CXXFLAGS += /wd4287 # unsigned/negative constant mismatch (An unsigned variable was used in an operation with a negative number.)
QMAKE_CXXFLAGS += /wd4296 # expression is always false
QMAKE_CXXFLAGS += /wd4355 # 'this' : used in base member initializer list
QMAKE_CXXFLAGS += /wd4437 # dynamic_cast from virtual base 'class1' to 'class2' could fail in some contexts Compile with /vd2 or define 'class2' with #pragma vtordisp(2) in effect
QMAKE_CXXFLAGS += /wd4738 # storing 32-bit float result in memory, possible loss of performance
QMAKE_CXXFLAGS += /we4546 # function call before comma missing argument list
QMAKE_CXXFLAGS += /we4172 # returning address of local variable or temporary
QMAKE_CXXFLAGS += /we4102 # unreferenced label
QMAKE_CXXFLAGS += /wd4522

QMAKE_CXXFLAGS += /openmp


QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings


    CONFIG(release, debug|release) {
        RCE_RETAIL_BUILD {
            # enable (and rebuild) this when shipping top performance build
            QMAKE_CXXFLAGS += /GL # https://msdn.microsoft.com/en-us/library/0zza0de8.aspx
            QMAKE_LFLAGS += /LTCG #https://msdn.microsoft.com/en-us/library/xbf3tbeh.aspx
        }
    }

}


# decide build machine
RCE_MACHINE_NAME = $$(RCE_BUILD_MACHINE)

    #RCE_ADAM_NTB_WIN = ADAM_NTB_WIN
    #RCE_DAVID_NTB_WIN = DAVID_NTB_WIN
    #RCE_PAVEL_PC_KOLEJE_WIN = PAVEL_PC_KOLEJE_WIN
    #RCE_PAVEL_PC_PRACA_WIN = PAVEL_PC_PRACA_WIN

isEmpty(RCE_MACHINE_NAME){
  #building new machine
  error("Missing RCE_BUILD_MACHINE system variable. Please set proper RCE_BUILD_MACHINE and add configuration to project DFSProjectConfiguration.")
  # in case, this error pops up, please, set your system variable (in Start-Computer-Properties-Advanced System Settings-Environment Variables
  # RCE_MACHINE_NAME to some meaningfull identification and add your library stuff as follows...
}


# add libraries
equals(RCE_MACHINE_NAME, ADAM_NTB_WIN){
  message("Building on Adams notebook.")
  include($$PWD/LibraryConfigsAdam.pri)
}

