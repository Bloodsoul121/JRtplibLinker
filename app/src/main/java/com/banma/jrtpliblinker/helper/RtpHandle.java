package com.banma.jrtpliblinker.helper;

import java.io.IOException;
import java.net.ServerSocket;

public class RtpHandle {

    /**
     * 生成一个可使用的udp端口（20000以上）
     *
     * @param port 传20000即可
     * @return
     */
    public int getAvailablePort(int port) {
        try {
            ServerSocket socket = new ServerSocket(port);
            socket.close();
            return port;
        } catch (IOException e) {
            return getAvailablePort(port + 2);
        }
    }

    /**
     * 只发送数据，不做接收处理
     */
    public native long initSendHandle(int localport, String desthost, int destport,
                                      RtpListener listener);

    /**
     * 接收数据，并转发
     */
    public native long initReceiveAndSendHandle(String localhost, int localreceiveport,
                                                int localsendport, String desthost, int destport,
                                                RtpListener listener);

    /**
     * 发送数据
     */
    public native boolean sendByte(long rtpHandler, byte[] src, int byteLength,
                                   boolean isSpsOrMarker, boolean isRtpData, long lastTime);

    /**
     * 销毁资源
     */
    public native boolean finiHandle(long rtpHandler);

}
