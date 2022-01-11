package com.banma.jrtpliblinker

import android.content.Intent
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.banma.jrtpliblinker.databinding.ActivityMainBinding
import com.banma.jrtpliblinker.reveive.ReceiveActivity

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.btnReceiveRtp.setOnClickListener {
            val result = stringFromJNI()
            Toast.makeText(this, result, Toast.LENGTH_SHORT).show()
        }

        startActivity(Intent(this, ReceiveActivity::class.java))
    }

    external fun stringFromJNI(): String

    companion object {
        init {
            System.loadLibrary("jrtplib")
        }
    }
}