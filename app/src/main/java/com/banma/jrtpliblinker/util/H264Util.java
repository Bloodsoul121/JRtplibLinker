package com.banma.jrtpliblinker.util;

import android.content.Context;
import android.os.Environment;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;

public class H264Util {

    private static final String TAG = "FileUtil";

    public static File getParentDir() {
        return Environment.getExternalStorageDirectory();
    }

    public static void writeContent(byte[] array, File saveFile) {
        char[] HEX_CHAR_TABLE = {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };
        StringBuilder sb = new StringBuilder();
        for (byte b : array) {
            // 高位
            sb.append(HEX_CHAR_TABLE[(b & 0xf0) >> 4]);
            // 低位
            sb.append(HEX_CHAR_TABLE[b & 0x0f]);
        }
        Log.i(TAG, "writeContent: " + sb.toString());
        if (saveFile == null) {
            return;
        }
        FileWriter writer = null;
        try {
            // 打开一个写文件器，构造函数中的第二个参数true表示以追加形式写文件
            writer = new FileWriter(saveFile, true);
            writer.write(sb.toString());
            writer.write("\n");
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void writeBytes(byte[] array, File saveFile) {
        FileOutputStream writer = null;
        try {
            // 打开一个写文件器，构造函数中的第二个参数true表示以追加形式写文件
            writer = new FileOutputStream(saveFile, true);
            writer.write(array);
            writer.write('\n');
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void copyAssetsFile(Context context, String assetsFileName, File dir,
                                      String fileName) {
        FileOutputStream fos = null;
        InputStream is = null;
        try {
            if (!dir.exists()) {
                dir.mkdirs();
            }

            File copyFile = new File(dir, fileName);

            if (copyFile.exists()) {
                copyFile.delete();
            }
            try {
                copyFile.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }

            fos = new FileOutputStream(copyFile);
            is = context.getAssets().open(assetsFileName);

            byte[] bytes = new byte[1024];
            int len;

            while ((len = is.read(bytes)) != -1) {
                fos.write(bytes, 0, len);
                fos.flush();
            }

            if (Looper.myLooper() == Looper.getMainLooper()) {
                Toast.makeText(context, "拷贝成功", Toast.LENGTH_SHORT).show();
            }

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    //删除文件夹和文件夹里面的文件
    public static void deleteDir(final String pPath) {
        File dir = new File(pPath);
        deleteDirWihFile(dir);
    }

    public static void deleteDirWihFile(File dir) {
        if (dir == null || !dir.exists() || !dir.isDirectory()) return;
        for (File file : dir.listFiles()) {
            if (file.isFile()) file.delete(); // 删除所有文件
            else if (file.isDirectory()) deleteDirWihFile(file); // 递规的方式删除文件夹
        }
        dir.delete();// 删除目录本身
    }

}
