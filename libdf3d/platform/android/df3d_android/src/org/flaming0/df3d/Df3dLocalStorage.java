package org.flaming0.df3d;

import android.app.Activity;
import android.content.Context;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.charset.Charset;

public class Df3dLocalStorage {
    private Activity activity;

    Df3dLocalStorage(Activity activity) {
        this.activity = activity;
    }

    public boolean writeToInternalFile(String filename, byte[] data) {
        FileOutputStream outputStream;

        try {
            outputStream = activity.openFileOutput(filename, Context.MODE_PRIVATE);
            outputStream.write(data);
            outputStream.close();

            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    public byte[] readInternalFile(String filename) {
        FileInputStream inputStream;

        try {
            inputStream = activity.openFileInput(filename);
            byte fileContent[] = new byte[(int)inputStream.getChannel().size()];
            inputStream.read(fileContent);
            inputStream.close();
            return fileContent;
        } catch (FileNotFoundException e) {
            return null;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
}
