#include <RtpReceiver.h>
#include <ctime>

#define H264 96
#define SSRC 100

ReceiveCallback::ReceiveCallback(_JNIEnv *env, jobject obj) {
    jobj = obj;
    jclass clz = env->GetObjectClass(jobj);
    if (!clz) {
        loge("get jclass wrong");
        return;
    }
    jmid_rtp = env->GetMethodID(clz, "receiveRtpData", "([BIZJ)V");
    if (!jmid_rtp) {
        loge("get jmethodID jmid_rtp wrong");
        return;
    }
    jmid_rtcp = env->GetMethodID(clz, "receiveRtcpData", "(Ljava/lang/String;)V");
    if (!jmid_rtcp) {
        loge("get jmethodID jmid_rtcp wrong");
        return;
    }
    jmid_bye = env->GetMethodID(clz, "receiveBye", "(Ljava/lang/String;)V");
    if (!jmid_bye) {
        loge("get jmethodID jmid_bye wrong");
        return;
    }
}

//回调java监听
void callbackRtcp(JNIEnv *env, int r_type, uint32_t r_ip, ReceiveCallback *hid_callback) {
    if (nullptr != env) {
        //没有主动调用停止，才可以回调数据
        if (r_type == 1) {
            char *string = changeIP(r_ip);
            env->CallVoidMethod(hid_callback->jobj, hid_callback->jmid_rtcp,
                                env->NewStringUTF(string));
        } else if (r_type == 2) {
            char *string = changeIP(r_ip);
            env->CallVoidMethod(hid_callback->jobj, hid_callback->jmid_bye,
                                env->NewStringUTF(string));
        }
    }
}

//回调java监听
void callbackRtp(JNIEnv *env, uint8_t *buff, int packet_length, bool isMarker, long m_time,
                 ReceiveCallback *hid_callback) {
    if (nullptr != env) {
        //没有主动调用停止，才可以回调数据
        jbyteArray jbarray = env->NewByteArray(packet_length);
        env->SetByteArrayRegion(jbarray, 0, packet_length, (jbyte *) buff);
        env->CallVoidMethod(hid_callback->jobj, hid_callback->jmid_rtp, jbarray,
                            packet_length, isMarker, m_time);
        env->DeleteLocalRef(jbarray);
    }
}

bool
CRTPReceiver::init(JavaVM *vm, JNIEnv *env, CRTPReceiver *receiver, const char *localip,
                   uint16_t PORT_BASE,
                   jobject listener) {
    logd("CRTPReceiver init jni!\n");
    logd("CRTPReceiver init ip %s , port %d\n", localip, PORT_BASE);
    if (m_init) {
        loge("CRTPReceiver init failed. already inited. \n");
        return false;
    }
    if (g_jobj == nullptr) {
        g_jobj = env->NewGlobalRef(listener);
    }
    r_env = env;
    r_vm = vm;
    hid_callback = new ReceiveCallback(env, g_jobj);
    env->GetJavaVM(&r_vm);//保存全局变量
    int status;
    // Now, we'll create a RTP session, set the destination
    // and poll for incoming data.
    RTPUDPv4TransmissionParams transParams;
    RTPSessionParams sessionParams;
    if (sessionParams.SetUsePollThread(true) < 0) {
        loge("CRTPReceiver init failed. SetUsePollThread error. \n");
        return false;
    }
    /* set h264 param */
//    sessionParams.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC
    sessionParams.SetOwnTimestampUnit(1.0 / 10.0); /* 设置采样间隔 */
    sessionParams.SetAcceptOwnPackets(false);   //接收自己发送的数据包
//    sessionParams.SetPredefinedSSRC(SSRC);     //定义SSRC
//    uint32_t local_ip = inet_addr(localip);
//    local_ip = ntohl(local_ip);
//    transParams.SetBindIP(local_ip);
    transParams.SetPortbase(PORT_BASE);
//    logd("CRTPReceiver transParams init ip %d , port %d\n", local_ip, PORT_BASE);
    status = receiver->Create(sessionParams, &transParams);
    if (status < 0) {
        loge("CRTPReceiver init failed. Create(sessionParams, &transParams)\n");
        return false;
    }
    CheckError(status);
    m_init = true;
    receiver->SetDefaultTimestampIncrement(3600);/* 设置时间戳增加间隔 */
    logd("CRTPReceiver init jni ok.\n");
    return true;
}

bool CRTPReceiver::fini(JNIEnv *env) {
    logd("CRTPReceiver fini jni!\n");
    m_init = false;
//    if (g_jobj && env) {
//        env->DeleteGlobalRef(g_jobj);
//        loge("DeleteGlobalRef.\n");
//    }
    BYEDestroy(RTPTime(3, 0), nullptr, 0);
    logd("CRTPReceiver fini jni ok.\n");
    return true;
}

void CRTPReceiver::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime,
                                        const RTPAddress *senderaddress) {
    const auto *addr = (const RTPIPv4Address *) (senderaddress);
    uint32_t rtcpIp = addr->GetIP();
    uint32_t port = addr->GetPort();
    logd("OnRTCP. ip:0x%x port:%d\n", rtcpIp, port);
    if (m_init && isAttach) {
        callbackRtcp(r_env, 1, rtcpIp, hid_callback);
    }
}

void CRTPReceiver::OnPollThreadStep() {
    logd("CRTPReceiver  OnPollThreadStep\n");
    BeginDataAccess();
    // check incoming packets
    if (GotoFirstSourceWithData()) {
        do {
            RTPPacket *rtppack;
            while ((rtppack = GetNextPacket()) != nullptr) {
                processRtpPacket(rtppack);
                DeletePacket(rtppack);
            }
        } while (GotoNextSourceWithData());
    }
    EndDataAccess();
}

void CRTPReceiver::OnPollThreadStart(bool &stop) {
    logd("CRTPReceiver  OnPollThreadStart\n");
    int status = r_vm->AttachCurrentThread(&r_env, nullptr);
    if (status < 0) {
        isAttach = false;
        r_env = nullptr;
        loge("CRTPReceiver AttachCurrentThread Error!\n");
        return;
    }
    isAttach = true;
    logd("CRTPReceiver AttachCurrentThread Ok!\n");
}

void CRTPReceiver::OnPollThreadStop() {
    logd("CRTPReceiver  OnPollThreadStop\n");
    //如果线程没有停止，就调用Detach，会报错。
    isAttach = false;
    if (r_env != nullptr) {
        int status = r_vm->DetachCurrentThread();
        if (status < 0) {
            loge("CRTPReceiver DetachCurrentThread Error!\n");
        } else {
            logd("CRTPReceiver DetachCurrentThread Ok!\n");
        }
        r_env = nullptr;
    }
}

void CRTPReceiver::OnNewSource(RTPSourceData *srcdat) {
    processSourceData(srcdat, "CRTPReceiver OnNewSource", true);
}

void CRTPReceiver::OnRemoveSource(RTPSourceData *srcdat) {
    processSourceData(srcdat, "CRTPReceiver OnRemoveSource", false);
}

void CRTPReceiver::OnBYEPacket(RTPSourceData *srcdat) {
    processSourceData(srcdat, "CRTPReceiver OnBYEPacket", false);
}

void CRTPReceiver::processSourceData(RTPSourceData *srcdat, const char *funcName, bool add) {
    logd("CRTPReceiver processSourceData.\n");
    if (funcName == nullptr) {
        loge("CRTPReceiver processSourceData error. \n");
        return;
    }
    if (!srcdat) {
        loge("%s.CRTPReceiver error \n", funcName);
        return;
    }
//    if (srcdat->IsOwnSSRC()) {
//        loge("%s CRTPReceiver error. is own ssrc \n", funcName);
//        return;
//    }
    uint32_t ip;
    uint16_t port;
    if (srcdat->GetRTPDataAddress() != nullptr) {
        const auto *addr = (const RTPIPv4Address *) (srcdat->GetRTPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort();
        logd("%s CRTPReceiver RTP. ip:0x%x port:%d\n", funcName, ip, port);
    } else if (srcdat->GetRTCPDataAddress() != nullptr) {
        const auto *addr = (const RTPIPv4Address *) (srcdat->GetRTCPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort();
        logd("%s CRTPReceiver RTCP. ip:0x%x port:%d\n", funcName, ip, port);
        port = port - 1;
    } else {
        loge("%s CRTPReceiver RTP/RTCP. error \n", funcName);
        return;
    }
//    if (ip != r_remoteIp || port != r_remotePort) {
//        logd("%s-- msg-ip:0x%x port:%d remote-ip:0x%x port:%d\n", funcName, ip, port,
//             r_remoteIp, r_remotePort);
//        return;
//    }
    RTPIPv4Address dest(ip, port);
    int32_t ret;
    if (add) {
        ret = AddDestination(dest);
    } else {
        if (m_init && isAttach) {
            callbackRtcp(r_env, 2, ip, hid_callback);
        }
        ret = DeleteDestination(dest);
    }

    if (ret < 0) {
        logd("%s %s error. ip:0x%x port:%d\n", funcName,
             (add ? "AddDestination" : "DeleteDestination"), ip, port);
    } else {
        logd("%s %s ok. ip:0x%x port:%d\n", funcName,
             (add ? "AddDestination" : "DeleteDestination"), ip, port);
    }
}

void CRTPReceiver::processRtpPacket(const RTPPacket *pack) {
    logd("CRTPReceiver processRtpPacket. \n");
    if (pack) {
        uint16_t seq = pack->GetSequenceNumber();
        if (0 == ((++m_count) % 500)) {
            m_count = 0;
            logd("CRTPReceiver processRtpPacket cur seq:%d. \n", seq);
        }
        if (m_firstSeq) {
            m_firstSeq = false;
            logd("CRTPReceiver processRtpPacket cur seq:%d. \n", seq);
        } else {
            uint16_t tmp = m_lastSeq;
            if ((tmp + 1) != seq) {
                logd("CRTPReceiver lost packet!! last seq:%d, cur seq:%d\n", m_lastSeq, seq);
            }
        }
        m_lastSeq = seq;
        logd("CRTPReceiver processRtpPacket GetPayloadType %d , m_init %d, isAttach %d. \n",
             pack->GetPayloadType(), m_init, isAttach);
//        if (pack->GetPayloadType() == H264 && m_init && isAttach) {
        if (m_init && isAttach) {
            struct timeval tv{};
            gettimeofday(&tv, nullptr);
            long currentTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            //std::cout<<"Got H264 packet：êo " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() <<std::endl;
            callbackRtp(r_env, pack->GetPayloadData(), pack->GetPayloadLength(), pack->HasMarker(),
                        currentTime,
                        hid_callback);
        }
    } else {
        logd("CRTPReceiver processRtpPacket pack err. \n");
    }
}
