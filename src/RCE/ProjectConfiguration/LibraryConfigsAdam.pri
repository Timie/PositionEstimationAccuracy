
RCE_MSVC{

    contains(RCE_REQUIRED_LIBS, TCLAP){
        INCLUDEPATH += "D:/ADAM/SelectedLibraries/tclap-1.2.1/include"
    }

    contains(RCE_REQUIRED_LIBS, EIGEN){
        INCLUDEPATH += "C:/Eigen/SOURCE"
    }

#contains(RCE_REQUIRED_LIBS, OPENGM){
#INCLUDEPATH += "C:/OpenGM/source/include"
#}

    CONFIG(release, debug|release) { #release





    #contains(RCE_REQUIRED_LIBS, DXF){
    #INCLUDEPATH += "C:/DXFLib"
    #LIBS += -L"C:/DXFLib"
    #LIBS += -ldxflib
    #}

        RCE_TARGET_ARCH_x64{

            contains(RCE_REQUIRED_LIBS, OPENSSL){
                message("Including OpenSSL")
                LIBS += -L"C:/OpenSSL-Win64/lib" -llibeay32 -lssleay32
            }

            contains(RCE_REQUIRED_LIBS, OPENCV){
                RCE_USE_OPENCV_WORLD {

                    message("Using Prebuilt OpenCV 3 (64b) world.")
                    #OpenCV 3.0.0
                    INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/opencv"
                    INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/"

                    LIBS += -L"C:/OpenCV3Precomp/opencv/build/x64/vc12/lib"
                    LIBS += -lopencv_world300

                } else {

                    message("Using Custom OpenCV 3 (64b).")
                    #OpenCV 3.0.0
                    INCLUDEPATH += "C:/OpenCV3Custom/build/install/include/opencv"
                    INCLUDEPATH += "C:/OpenCV3Custom/build/install/include/"

                    DEFINES += RCE_EXPERIMENTAL_OPENCV

                    #LIBS += -L"C:/OpenCV3Precomp/opencv/build/x64/vc12/staticlib"
                    #LIBS += -lopencv_calib3d300d \
                    #        -lopencv_core300d \
                    #        -lopencv_features2d300d \
                    #        -lopencv_hal300d \
                    #        -lopencv_highgui300d \
                    #        -lopencv_imgcodecs300d \
                    #        -lopencv_imgproc300d \
                    #        -lopencv_objdetect300d \
                    #        -lopencv_shape300d \
                    #        -lopencv_video300d \
                    #        -lopencv_videoio300d \
                    #        -lopencv_videostab300d \
                    #        -lzlib
                    LIBS += -L"C:/OpenCV3Custom/build/install/x64/vc12/lib"
                    #LIBS += -lopencv_world300d
                    LIBS += -lopencv_calib3d300 \
                            -lopencv_core300 \
                            -lopencv_features2d300 \
                            -lopencv_hal300 \
                            -lopencv_highgui300 \
                            -lopencv_imgcodecs300 \
                            -lopencv_imgproc300 \
                            -lopencv_objdetect300 \
                            -lopencv_shape300 \
                            -lopencv_video300 \
                            -lopencv_videoio300 \
                            -lopencv_videostab300 \
                            -lopencv_xfeatures2d300
                     }
                }

           contains(RCE_REQUIRED_LIBS, CRYPTOPP){
                    INCLUDEPATH += "D:/crypto2/include/"
                    LIBS += -L"D:/crypto2/x64/Output/Release/" -lcryptlib
           }
        }

        RCE_TARGET_ARCH_x86{

            contains(RCE_REQUIRED_LIBS, OPENCV){
                message("Using Prebuilt OpenCV 3.0 (32b).")
                #OpenCV 2.4.7
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/opencv"
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/"

                LIBS += -L"C:/OpenCV3Precomp/opencv/build/x86/vc12/lib"
                LIBS += -lopencv_world300
            }

            contains(RCE_REQUIRED_LIBS, CRYPTOPP){
                INCLUDEPATH += "D:/crypto2/include/"
                LIBS += -L"D:/crypto2/Win32/Output/Release/" -lcryptlib
            }
        }
    }

    CONFIG(debug, debug|release) { #debug



            #contains(RCE_REQUIRED_LIBS, DXF){
            #INCLUDEPATH += "C:/DXFLib"
            #LIBS += -L"C:/DXFLib/debug"
            #LIBS += -ldxflib
            #}

            RCE_TARGET_ARCH_x64{


#            message("Including OpenSSL")
#            LIBS += -L"C:/OpenSSL-Win64/lib" -llibeay32 -lssleay32

            contains(RCE_REQUIRED_LIBS, OPENCV){

                message("Using Precomp OpenCV 3 (64b).")
                #OpenCV 3.0.0
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/opencv"
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/"

                #LIBS += -L"C:/OpenCV3Precomp/opencv/build/x64/vc12/staticlib"
                #LIBS += -lopencv_calib3d300d \
                #        -lopencv_core300d \
                #        -lopencv_features2d300d \
                #        -lopencv_hal300d \
                #        -lopencv_highgui300d \
                #        -lopencv_imgcodecs300d \
                #        -lopencv_imgproc300d \
                #        -lopencv_objdetect300d \
                #        -lopencv_shape300d \
                #        -lopencv_video300d \
                #        -lopencv_videoio300d \
                #        -lopencv_videostab300d \
                #        -lzlib
                LIBS += -L"C:/OpenCV3Precomp/opencv/build/x64/vc12/lib"
                LIBS += -lopencv_world300d
            }

            contains(RCE_REQUIRED_LIBS, CRYPTOPP){
                INCLUDEPATH += "D:/crypto2/include/"
                LIBS += -L"D:/crypto2/x64/Output/Debug/" -lcryptlibd
            }
        }




        RCE_TARGET_ARCH_x86{



            contains(RCE_REQUIRED_LIBS, OPENCV){

            message("Using Prebuilt OpenCV 3.0 (32b).")
                #OpenCV 2.4.7
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/opencv"
                INCLUDEPATH += "C:/OpenCV3Precomp/opencv/build/include/"

                LIBS += -L"C:/OpenCV3Precomp/opencv/build/x86/vc12/lib"
                LIBS += -lopencv_world300d
            }

            contains(RCE_REQUIRED_LIBS, CRYPTOPP){
                INCLUDEPATH += "D:/crypto2/include/"
                LIBS += -L"D:/crypto2/Win32/Output/Debug/" -lcryptlibd
            }
        }
    }
}
