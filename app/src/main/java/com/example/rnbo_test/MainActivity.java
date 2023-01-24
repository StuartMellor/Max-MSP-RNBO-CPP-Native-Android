package com.example.rnbo_test;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSeekBar;

import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.SeekBar;
import android.view.View;
import android.widget.TextView;

import com.example.rnbo_test.databinding.ActivityMainBinding;

import org.w3c.dom.Text;

import java.util.Map;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'rnbo_test' library on application startup.
    static {
        System.loadLibrary("rnbo_test");
    }

    // Main app layout binding.
    private ActivityMainBinding binding;
    // Map of all parameter sliders!
    // This could become more complex in the
    // future through dials, etc.
    // For now, I've hard coded exposed RNBO params.
    // It shouldn't be too difficult to receive the params
    // from the RNBO patch.

    private Map<String, AppCompatSeekBar> sliders;
    private LinearLayout paramHolder;
    private SliderListener msliderListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        paramHolder = binding.ParamHolder;

        // Initialise the rnbo patch
        init();
    }

    private class SliderListener implements SeekBar.OnSeekBarChangeListener {

        public void onProgressChanged(SeekBar seekBar, int progress,
                                      boolean fromUser) {
            Log.d("DEBUG", "Progress is: " + progress + " for " + seekBar.getTag());
            updateParam((String) seekBar.getTag(), (float) progress);

        }

        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        public void onStopTrackingTouch(SeekBar seekBar) {
        }

    }


    public void declareSliders(@NonNull String[] paramNames) {
        // Array of parameter names from built RNBO Patch and an array of
        // TODO Add initial values so sliders are already set!
        // initial values that are scaled to between 0 and 1000;
//        String[] paramNames = paramNamesCombined.split(",");
        Log.i("MyApp", "Number of Params: " + paramNames.length);
        msliderListener = new SliderListener();
        for (String name : paramNames) {
            Log.i("MyApp", "param name: " + name);
            TextView label = new TextView(this);
            label.setText(name);
            paramHolder.addView(label);

            AppCompatSeekBar seekBar = new AppCompatSeekBar(this);
            seekBar.setTag(name);
            seekBar.setMax(1000);
            seekBar.setOnSeekBarChangeListener(msliderListener);
            seekBar.setProgress(500);

            paramHolder.addView(seekBar, new LayoutParams(LayoutParams.MATCH_PARENT, 250));
        }
    }


    void updateParameter(String name, float val) {
        // Java to c++ function to update RNBO patch.
    }

    public native void init();
    // Again, val is scaled between 0 and 1000, will need to be normalised.
    public native void updateParam(String name, float val);

}