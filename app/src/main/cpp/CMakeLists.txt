cmake_minimum_required(VERSION 3.10.2)

project("jrtpliblinker")

include_directories(../jni)
include_directories(../jni/jthread)
include_directories(jrtplib3/src)
include_directories(../jni/rtphelper)

#file(GLOB cpp_source2 ../jni/jrtplib3/*.cpp)
#file(GLOB cpp_source3 ../jni/jthread/*.cpp)
#file(GLOB cpp_source4 ../jni/rtphelper/*.cpp)

add_library(jrtplib SHARED

#        ${cpp_source2}
#        ${cpp_source3}
#        ${cpp_source4}

        ../jni/jthread/jmutex.cpp
        ../jni/jthread/jthread.cpp

        ../jni/jrtplib3/src/rtcpapppacket.cpp
        ../jni/jrtplib3/src/rtcpbyepacket.cpp
        ../jni/jrtplib3/src/rtcpcompoundpacket.cpp
        ../jni/jrtplib3/src/rtcpcompoundpacketbuilder.cpp
        ../jni/jrtplib3/src/rtcppacket.cpp
        ../jni/jrtplib3/src/rtcppacketbuilder.cpp
        ../jni/jrtplib3/src/rtcprrpacket.cpp
        ../jni/jrtplib3/src/rtcpscheduler.cpp
        ../jni/jrtplib3/src/rtcpsdesinfo.cpp
        ../jni/jrtplib3/src/rtcpsdespacket.cpp
        ../jni/jrtplib3/src/rtcpsrpacket.cpp
        ../jni/jrtplib3/src/rtpcollisionlist.cpp
        ../jni/jrtplib3/src/rtpdebug.cpp
        ../jni/jrtplib3/src/rtperrors.cpp
        ../jni/jrtplib3/src/rtpinternalsourcedata.cpp
        ../jni/jrtplib3/src/rtpipv4address.cpp
        ../jni/jrtplib3/src/rtpipv6address.cpp
        ../jni/jrtplib3/src/rtpipv4destination.cpp
        ../jni/jrtplib3/src/rtpipv6destination.cpp
        ../jni/jrtplib3/src/rtplibraryversion.cpp
        ../jni/jrtplib3/src/rtppacket.cpp
        ../jni/jrtplib3/src/rtppacketbuilder.cpp
        ../jni/jrtplib3/src/rtppollthread.cpp
        ../jni/jrtplib3/src/rtprandom.cpp
        ../jni/jrtplib3/src/rtprandomrand48.cpp
        ../jni/jrtplib3/src/rtprandomrands.cpp
        ../jni/jrtplib3/src/rtprandomurandom.cpp
        ../jni/jrtplib3/src/rtpsession.cpp
        ../jni/jrtplib3/src/rtpsessionparams.cpp
        ../jni/jrtplib3/src/rtpsessionsources.cpp
        ../jni/jrtplib3/src/rtpsourcedata.cpp
        ../jni/jrtplib3/src/rtpsources.cpp
        ../jni/jrtplib3/src/rtptimeutilities.cpp
        ../jni/jrtplib3/src/rtpudpv4transmitter.cpp
        ../jni/jrtplib3/src/rtpudpv6transmitter.cpp
        ../jni/jrtplib3/src/rtpbyteaddress.cpp
        ../jni/jrtplib3/src/rtpexternaltransmitter.cpp
        ../jni/jrtplib3/src/rtpsecuresession.cpp
        ../jni/jrtplib3/src/rtpabortdescriptors.cpp
        ../jni/jrtplib3/src/rtptcpaddress.cpp
        ../jni/jrtplib3/src/rtptcptransmitter.cpp

        ../jni/rtphelper/RtpCommon.cpp
        ../jni/rtphelper/RtpHandle.cpp
        ../jni/rtphelper/RtpSender.cpp
        ../jni/rtphelper/RtpReceiver.cpp

        native-lib.cpp
        )

target_link_libraries(
        jrtplib
        log
        android
)