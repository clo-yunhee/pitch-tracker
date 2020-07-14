package fr.cloyunhee.pitchtracker;

import org.libsdl.app.SDLActivity;
import android.os.Vibrator;
import android.os.VibrationEffect;
import android.content.Context;
import android.util.Log;

public class PitchTracker extends SDLActivity {

    private static final String TAG = "PitchTracker";

    public void vibeStart(long duration)
    {
        final Vibrator vibe = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (vibe != null && vibe.hasVibrator()) {
            vibe.cancel();
            vibe.vibrate(VibrationEffect.createOneShot(duration, 64));
        }
    }

    public void vibeStop()
    {
        final Vibrator vibe = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (vibe != null && vibe.hasVibrator()) {
            vibe.cancel();
        }
    }

}
