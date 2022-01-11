package com.banma.jrtpliblinker.reveive

import android.content.SharedPreferences
import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.banma.jrtpliblinker.databinding.ActivityReceiveBinding
import com.banma.jrtpliblinker.helper.Constants
import com.banma.jrtpliblinker.helper.IpCameraHelper
import com.banma.jrtpliblinker.util.LogUtil
import com.banma.jrtpliblinker.util.NetworkUtil

class ReceiveActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "ReceiveActivity"
    }

    private lateinit var binding: ActivityReceiveBinding

    //是否使用RTSP
    private var useRtsp = false

    private var sp: SharedPreferences? = null

    private var ipCameraHelper: IpCameraHelper? = null
    private var ipCameraHelper2: IpCameraHelper? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityReceiveBinding.inflate(layoutInflater)
        setContentView(binding.root)
        initView()
        initListener()
    }

    private fun initView() {
        binding.tvContent.movementMethod = ScrollingMovementMethod()
        binding.tvContent.keepScreenOn = true
        if (sp == null) {
            sp = getPreferences(MODE_PRIVATE)
            val ip = sp!!.getString("ip", "")
            val rtsp = sp!!.getString("rtsp", "")
            if (ip!!.isNotEmpty()) {
                binding.etDest.setText(ip)
            }
            if (rtsp!!.isNotEmpty()) {
                binding.etRtsp.setText(rtsp)
            }
        }
        binding.cbRtsp.setOnCheckedChangeListener { _, isChecked ->
            useRtsp = isChecked
            if (isChecked) {
                binding.etRtsp.visibility = View.VISIBLE
            } else {
                binding.etRtsp.visibility = View.GONE
            }
        }
    }

    private fun initListener() {
        binding.btnStart.setOnClickListener(View.OnClickListener {
            if (binding.btnStart.text == "开始") {
                try {
                    val s: String = binding.etDest.text.toString().trim()
                    if (s.isNotEmpty()) {
                        val tmp = s.split(":").toTypedArray()
                        Constants.remoteIp = tmp[0]
                        Constants.remotePort = tmp[1].toInt()
                    }
                    val rtsp: String = binding.etRtsp.text.toString().trim()
                    if (sp == null) {
                        sp = getPreferences(MODE_PRIVATE)
                    }
                    val edit = sp!!.edit()
                    edit.putString("ip", s)
                    edit.putString("rtsp", rtsp)
                    edit.apply()
                } catch (e: Exception) {
                    e.printStackTrace()
                    return@OnClickListener
                }
                binding.btnStart.text = "停止"
                if (ipCameraHelper == null) {
                    val localIp = NetworkUtil.getInstance().getLocalIp(this@ReceiveActivity)
                    LogUtil.d(TAG, "getLocalIp $localIp")
                    ipCameraHelper = IpCameraHelper(localIp, Constants.localPort, Constants.remoteIp, Constants.remotePort)
                }
                //如果勾选了使用rtsp，则初始化rtsp
                if (useRtsp) {
                    val s: String = binding.etRtsp.text.toString().trim()
                    ipCameraHelper?.initRtspClient(s)
                }
                //否则使用默认的接收数据
                ipCameraHelper?.initData()
            } else if (binding.btnStart.text == "停止") {
                binding.btnStart.text = "开始"
                Thread { fini() }.start()
            }
        })
    }

    private fun fini() {
        if (ipCameraHelper != null) {
            ipCameraHelper?.fini()
            ipCameraHelper = null
        }
        if (ipCameraHelper2 != null) {
            ipCameraHelper2?.fini()
            ipCameraHelper2 = null
        }
    }

    override fun onDestroy() {
        fini()
        super.onDestroy()
    }

    /**
     * 添加内容至文本输出
     */
    private fun addDataToText(s: String) {
        runOnUiThread {
            val last: CharSequence = binding.tvContent.text.toString().trim { it <= ' ' }
            val buffer = StringBuffer(last)
            buffer.append("\r\n")
            buffer.append(System.currentTimeMillis())
            buffer.append(" - ")
            buffer.append(s)
            binding.tvContent.text = buffer.toString()
            val scrollAmount: Int = (binding.tvContent.layout.getLineTop(binding.tvContent.lineCount)
                    - binding.tvContent.height)
            if (scrollAmount > 0) {
                binding.tvContent.scrollTo(0, scrollAmount)
            } else {
                binding.tvContent.scrollTo(0, 0)
            }
        }
    }

}