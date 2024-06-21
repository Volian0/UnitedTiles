package com.volian.unitedtiles;

import org.libsdl.app.SDLActivity;

import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import android.content.Context;
import android.os.Bundle;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.android.gms.ads.appopen.AppOpenAd;
import android.content.Context;
import android.os.Bundle;
import android.app.Application;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.appopen.AppOpenAd.AppOpenAdLoadCallback;
import com.google.android.gms.ads.LoadAdError;
import android.app.Activity;
import androidx.annotation.NonNull;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.AdError;
import android.app.Application.ActivityLifecycleCallbacks;
//import androidx.appcompat.app.AppCompatActivity;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.RequestConfiguration;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicBoolean;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.RelativeLayout;
import android.widget.FrameLayout;
import android.widget.PopupMenu;
import android.widget.Toast;
import android.graphics.Color;
import android.view.ViewGroup; // Make sure to import ViewGroup
import android.view.SurfaceView;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;
import com.google.android.gms.ads.OnUserEarnedRewardListener;

import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.tasks.OnSuccessListener;
import android.content.Intent;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.gms.tasks.*;

import android.provider.Settings.System;
import android.provider.Settings;

import android.view.Window;

import com.vungle.ads.VunglePrivacySettings;
import com.vungle.ads.VunglePrivacySettings; 
import com.ironsource.mediationsdk.IronSource;
import com.google.android.gms.ads.RequestConfiguration;
import com.google.android.gms.ads.RequestConfiguration.Builder;
import java.util.List;

import com.google.android.gms.ads.ResponseInfo;
import com.google.android.gms.ads.AdapterResponseInfo;

public class UnitedTilesActivity extends SDLActivity {
    public AdView adViewB = null;
    public AdSize adSizeB = null;
    public boolean resizeOnce = false;
    public boolean showBigOnce = false;
    public boolean adLoaded = false;
    public boolean canUseSDK = false;
    public int resumedOnce = 0;
    public boolean test_device = false;
    //private static MinesweeperPillarActivity activity;

    public InterstitialAd mInterstitialAd;

    private static final int RC_LEADERBOARD_UI = 9004;

    public void resizeWindowAd() {
        if (!resizeOnce) {
            ViewGroup.LayoutParams gameLayoutParams = ((View)mSurface).getLayoutParams();
            gameLayoutParams.height = ((View)mSurface).getHeight() - adSizeB.getHeightInPixels(this);
 
            ((View)mSurface).setLayoutParams(gameLayoutParams);
            resizeOnce = true;
        }
    }

    public void loadBigAd() {
        if (test_device)
        {
            return;
        }
        if (adLoaded) {
            return;
        }
        adLoaded = true;
        mInterstitialAd = null;

        AdRequest adRequest = new AdRequest.Builder().build();

        InterstitialAd.load(this, "ca-app-pub-9020041895455560/8092335055", adRequest,
        //InterstitialAd.load(this, "ca-app-pub-3940256099942544/1033173712", adRequest,
            new InterstitialAdLoadCallback() {
                @Override
                public void onAdLoaded(@NonNull InterstitialAd interstitialAd) {
                    // The mInterstitialAd reference will be null until
                    // an ad is loaded.
                                                Log.d("L3 IronSource", "Ad is now loaded.");

                    mInterstitialAd = interstitialAd;

                    mInterstitialAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                        @Override
                        public void onAdClicked() {
                            // Called when a click is recorded for an ad.
                        }

                        @Override
                        public void onAdDismissedFullScreenContent() {
                            // Called when ad is dismissed.
                            // Set the ad reference to null so you don't show the ad a second time.
                            mInterstitialAd = null;
                        }

                        @Override
                        public void onAdFailedToShowFullScreenContent(AdError adError) {
                            // Called when ad fails to show.
                                        Log.d("L3 IronSource", "Ad failed to show");
                            mInterstitialAd = null;
                        }

                        @Override
                        public void onAdImpression() {
                            // Called when an impression is recorded for an ad.
                        }

                        @Override
                        public void onAdShowedFullScreenContent() {
                            Log.d("L3 IronSource", "Ad shown");

                            // Called when ad is shown.
                        }
                    });

                    if (!showBigOnce) {
                        showBigOnce = true;
                        //showBigAd();
                    }

                }

                @Override
                public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                    // Handle the error
                    mInterstitialAd = null;
                                                Log.d("L3 IronSource", "Ad is not loaded");

                }
            });
    }

    public void showBigAd() {
                if (test_device)
        {
            return;
        }
        adLoaded = false;
        if (mInterstitialAd != null) {
            mInterstitialAd.show(this);
        }
    }

    private boolean isTestDevice() {



  String testLabSetting = Settings.System.getString(getContentResolver(), "firebase.test.lab");
  return "true".equals(testLabSetting);
}


    public void showAd() {
                if (test_device)
        {
            return;
        }
        adViewB = new AdView(this);
        adViewB.setAdUnitId("ca-app-pub-9020041895455560/1718498398");
        //adViewB.setAdUnitId("ca-app-pub-3940256099942544/9214589741");
        adViewB.setAdSize(AdSize.BANNER);

        AdRequest adRequest = new AdRequest.Builder().build(); 

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WRAP_CONTENT,
            RelativeLayout.LayoutParams.WRAP_CONTENT);

        params.addRule(RelativeLayout.ALIGN_PARENT_TOP);

        params.addRule(RelativeLayout.CENTER_HORIZONTAL);

        adViewB.loadAd(adRequest);

        adViewB.setBackgroundColor(Color.TRANSPARENT);

        mLayout.addView(adViewB, params);

        adViewB.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                super.onAdLoaded();

                // Get the height of the ad in pixels
                int adHeight = adViewB.getHeight();
                adSizeB = adViewB.getAdSize();

                resizeWindowAd();
            }


            @Override
            public void onAdFailedToLoad(LoadAdError loadAdError) {
            ResponseInfo responseInfo = loadAdError.getResponseInfo();
            List<AdapterResponseInfo> adapterResponses = responseInfo.getAdapterResponses();
            Log.d("L1", responseInfo.toString());
            for (AdapterResponseInfo info : adapterResponses) {
                Log.d("L2", responseInfo.toString());
            }
            }


        });

    }

    public void testLoadAd() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                loadBigAd();
            }
        });
    }

    public void testShowAd() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                showBigAd();
            }
        });
    }

    public void testShare() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                
                Intent sendIntent = new Intent();
                sendIntent.setAction(Intent.ACTION_SEND);
                sendIntent.putExtra(Intent.EXTRA_TEXT, "https://play.google.com/store/apps/details?id=com.volian.unitedtiles");
                sendIntent.setType("text/plain");

                Intent shareIntent = Intent.createChooser(sendIntent, null);
                startActivity(shareIntent);


            }
        });
    }

    public void testShowRating() {
        Activity activity = this;
        this.runOnUiThread(new Runnable() {
                    

            @Override
            public void run() {
                
                ReviewManager manager = ReviewManagerFactory.create(activity);
                Task<ReviewInfo> request = manager.requestReviewFlow();
                request.addOnCompleteListener(task -> {
                    if (task.isSuccessful()) {
                        // We can get the ReviewInfo object
                        ReviewInfo reviewInfo = task.getResult();

                        Task<Void> flow = manager.launchReviewFlow(activity, reviewInfo);
                        flow.addOnCompleteListener(task2 -> {
                            // The flow has finished. The API does not indicate whether the user
                            // reviewed or not, or even whether the review dialog was shown. Thus, no
                            // matter the result, we continue our app flow.
                        });


                    } else {
                        // There was some problem, log or handle the error code.
                    }
                });

                                
                            }
        });
    }

    public void testShowBanner() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                showAd();
            }
        });
    }

    public void submitScore(String leaderboardId, long score)
    {
        if (!canUseSDK)
        {
            return;
        }
        Activity activity = this;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                PlayGames.getLeaderboardsClient(activity)
    .submitScore(leaderboardId, score);
            }
        });
    }

public void showLeaderboard(String leaderboardId)
    {
        if (!canUseSDK)
        {
            return;
        }
        Activity activity = this;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // Your UI update logic goes here
                PlayGames.getLeaderboardsClient(activity)
      .getLeaderboardIntent(leaderboardId)
      .addOnSuccessListener(new OnSuccessListener<Intent>() {
        @Override
        public void onSuccess(Intent intent) {
          startActivityForResult(intent, RC_LEADERBOARD_UI);
        }
      });

            }
        });
    }



@Override
public void onResume() {
    super.onResume();
    IronSource.onResume(this);
}

@Override
public void onPause() {
    super.onPause();
    IronSource.onPause(this);
}



    @Override
    public void onCreate(Bundle savedInstanceState) {

            //RequestConfiguration.Builder builder = new RequestConfiguration.Builder();
            //List<String> testDeviceIds = Arrays.asList("6C3B420CA4230D8D76C463A2B27DF1E7");
            //builder.setTestDeviceIds(testDeviceIds);
            //builder.build();


      VunglePrivacySettings.setGDPRStatus(true, "v1.0.0");


VunglePrivacySettings.setCCPAStatus(true);



IronSource.setConsent(true);


IronSource.setMetaData("do_not_sell", "true");


        adViewB = null;
        adSizeB = null;
        resizeOnce = false;
        showBigOnce = false;
        adLoaded = false;
        resumedOnce = 0;
        canUseSDK = false;
        test_device = isTestDevice();
        //test_device = true;
        //getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);


        //getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        MobileAds.initialize(this, new OnInitializationCompleteListener() {
            @Override
            public void onInitializationComplete(InitializationStatus initializationStatus) {



            }
        });
        PlayGamesSdk.initialize(this);
        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient(this);

        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
        boolean isAuthenticated =
            (isAuthenticatedTask.isSuccessful() &&
            isAuthenticatedTask.getResult().isAuthenticated());

        if (isAuthenticated) {
           canUseSDK = true;
        }
        });

    }
}