#include "AdManager.hpp"

#ifdef __ANDROID__

#include <SDL.h>
#include <jni.h>

#include <stdexcept>
#include <string_view>

extern "C"
{
    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_showLeaderboard(JNIEnv* env,
                                                                                                   jobject obj, std::string_view str)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "showLeaderboard", "(Ljava/lang/String;)V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        jstring jstr = env->NewStringUTF(str.data());
        env->CallVoidMethod(obj, methodId, jstr);
    }

    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_submitScore(JNIEnv* env,
                                                                                                   jobject obj, std::string_view str, long score)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "submitScore", "(Ljava/lang/String;J)V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        jstring jstr = env->NewStringUTF(str.data());
        jlong jscore = score;
        env->CallVoidMethod(obj, methodId, jstr, jscore);
    }

    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testShowAd(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testShowAd", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }
    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testLoadRewardedAd(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testLoadRewardedAd", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }

        JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testShowRewardedAd(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testShowRewardedAd", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }

        JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_handleReward(JNIEnv* env,
                                                                                                  jobject /* this */)
    {
        AdManager::m_reward = true;
    }


    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testShare(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testShare", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }

    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testShowBanner(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found");
        ; // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testShowBanner", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }

    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testLoadAd(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found"); // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testLoadAd", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }

    JNIEXPORT void JNICALL Java_com_volian_unitedtiles_UnitedTilesActivity_testShowRating(JNIEnv* env,
                                                                                                   jobject obj)
    {
        jclass clazz = env->FindClass("com/volian/unitedtiles/UnitedTilesActivity");
        if (clazz == nullptr)
            throw std::runtime_error("class not found"); // Class not found

        jmethodID methodId = env->GetMethodID(clazz, "testShowRating", "()V");
        if (methodId == nullptr)
            throw std::runtime_error("method not found"); // Method not found

        env->CallVoidMethod(obj, methodId);
    }
}

void call_java_function(std::string_view t_function)
{
    if (t_function == "load_big_ad")
    {
        Java_com_volian_unitedtiles_UnitedTilesActivity_testLoadAd((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity());
    }
    else if (t_function == "show_big_ad")
    {
        Java_com_volian_unitedtiles_UnitedTilesActivity_testShowAd((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity());
    }
    else if (t_function == "show_banner")
    {
        Java_com_volian_unitedtiles_UnitedTilesActivity_testShowBanner(
            (JNIEnv*)SDL_AndroidGetJNIEnv(), (jobject)SDL_AndroidGetActivity());
    }
    else if (t_function == "load_rewarded_ad")
    {
        Java_com_volian_unitedtiles_UnitedTilesActivity_testLoadRewardedAd((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                   (jobject)SDL_AndroidGetActivity());
    }
    else if (t_function == "show_rewarded_ad")
    {
        Java_com_volian_unitedtiles_UnitedTilesActivity_testShowRewardedAd((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                   (jobject)SDL_AndroidGetActivity());
    }
}

void submit_score(std::string_view t_leaderboard_id, long t_score)
{
          Java_com_volian_unitedtiles_UnitedTilesActivity_submitScore((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity(), t_leaderboard_id, t_score);
                                                                                
}
void show_leaderboard(std::string_view t_leaderboard_id)
{
        Java_com_volian_unitedtiles_UnitedTilesActivity_showLeaderboard((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity(), t_leaderboard_id);
}

void show_rating()
{
     Java_com_volian_unitedtiles_UnitedTilesActivity_testShowRating((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity());
}
void share_game() 
{
     Java_com_volian_unitedtiles_UnitedTilesActivity_testShare((JNIEnv*)SDL_AndroidGetJNIEnv(),
                                                                                (jobject)SDL_AndroidGetActivity());
}

#else
void call_java_function(std::string_view t_function)
{
}
void submit_score(std::string_view t_leaderboard_id, long t_score)
{

}
void show_leaderboard(std::string_view t_leaderboard_id)
{
    
}
void show_rating() 
{

}
void share_game() 
{

}
#endif

/*void AdManager::reward()
{
    call_java_function("reward");
}*/
[[nodiscard]] bool AdManager::is_rewarded()
{
    const bool rewarded = m_reward;
    if (rewarded)
    {
        m_reward = false;
    }
    return rewarded;
}

void AdManager::show_banner()
{
    if (!banner_showed)
    {
        call_java_function("show_banner");        
    }
    banner_showed = true;
}

AdManager::AdManager()
{
}

[[nodiscard]] bool AdManager::can_load_big_ad()
{
    return std::chrono::steady_clock::now() - m_last_big_ad_loaded >= COOLDOWN_LOAD_AD;
}

[[nodiscard]] bool AdManager::can_load_rewarded_ad()
{
    return std::chrono::steady_clock::now() - m_last_rewarded_ad_loaded >= COOLDOWN_LOAD_AD;
}

bool AdManager::load_big_ad()
{
    if (!can_load_big_ad())
    {
        return false;
    }
    if (m_is_big_ad_loaded)
    {
        return true;
    }
    call_java_function("load_big_ad");
    m_last_big_ad_loaded = std::chrono::steady_clock::now();
    m_is_big_ad_loaded = true;
    return true;
}
bool AdManager::show_big_ad()
{
    if (!can_show_big_ad())
    {
        return false;
    }
    if (taryfa_ulgowa)
    {
        taryfa_ulgowa = false;
        return false;
    }
    taryfa_ulgowa = true;
    m_is_big_ad_loaded = false;
    call_java_function("show_big_ad");
    m_last_big_ad_showed = std::chrono::steady_clock::now();
    return true;
}
bool AdManager::load_rewarded_ad()
{
    if (!can_load_rewarded_ad())
    {
        return false;
    }
    if (m_is_rewarded_ad_loaded)
    {
        return true;
    }
    call_java_function("load_rewarded_ad");
    m_is_rewarded_ad_loaded = true;
    m_last_rewarded_ad_loaded = std::chrono::steady_clock::now();
    return true;
}

bool AdManager::show_rewarded_ad()
{
    if (!can_show_rewarded_ad())
    {
        return false;
    }
    m_is_rewarded_ad_loaded = false;
    call_java_function("show_rewarded_ad");
    m_last_rewarded_ad_showed = std::chrono::steady_clock::now();
    return true;
}
[[nodiscard]] bool AdManager::can_show_big_ad()
{
#ifdef __ANDROID__
    return m_is_big_ad_loaded && std::chrono::steady_clock::now() - m_last_big_ad_showed >= COOLDOWN_BIG_AD && 
    std::chrono::steady_clock::now() - m_last_big_ad_loaded >= std::chrono::seconds(61);
#else
    return false;
#endif
}
[[nodiscard]] bool AdManager::can_show_rewarded_ad()
{
#ifdef __ANDROID__
    return m_is_rewarded_ad_loaded && std::chrono::steady_clock::now() - m_last_rewarded_ad_showed >= COOLDOWN_REWARDED_AD &&
           std::chrono::steady_clock::now() - m_last_rewarded_ad_loaded >= std::chrono::seconds(61);
#else
    return false;
#endif
}

/*void AdManager::show()
{
    const auto tp_now = std::chrono::steady_clock::now();
    if (tp_now - m_last_ad_showed < std::chrono::minutes(3)
    || tp_now - m_last_ad_loaded < std::chrono::seconds(15))
    {
        return;
    }
    m_last_ad_showed = tp_now;
    m_is_loaded = false;
    call_java_function("show");
}
void AdManager::load()
{
    if (m_is_loaded)
    {
        return;
    }
    const auto tp_now = std::chrono::steady_clock::now();
    if (tp_now - m_last_ad_showed < std::chrono::seconds(15))
    {
        return;
    }
    m_is_loaded = true;
    m_last_ad_loaded = std::chrono::steady_clock::now();
    call_java_function("load");
}
void AdManager::load_or_show()
{
    if (!m_is_loaded)
    {
        load();
    }
    else
    {
        show();
    }
}*/