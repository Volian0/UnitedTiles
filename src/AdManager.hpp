#pragma once

#include <atomic>
#include <chrono>
#include <string_view>

void submit_score(std::string_view t_leaderboard_id, long t_score);
void show_leaderboard(std::string_view t_leaderboard_id);
void show_rating();
void share_game();

class AdManager final
{
public:
    AdManager();
    /*void show();
    void load();
    void load_or_show();

    void reward();*/

    static constexpr auto COOLDOWN_BIG_AD{std::chrono::minutes{2}};
    static constexpr auto COOLDOWN_REWARDED_AD{std::chrono::seconds{30}};
    static constexpr auto COOLDOWN_LOAD_AD{std::chrono::seconds{11}};

    void show_banner();
    bool load_big_ad();
    bool show_big_ad();
    bool load_rewarded_ad();
    bool show_rewarded_ad();
    [[nodiscard]] bool can_show_big_ad();
    [[nodiscard]] bool can_show_rewarded_ad();

    [[nodiscard]] bool can_load_big_ad();
    [[nodiscard]] bool can_load_rewarded_ad();

    [[nodiscard]] bool is_rewarded();

    inline static std::atomic<bool> m_reward{false};

private:
    bool m_is_big_ad_loaded = false;
    bool m_is_rewarded_ad_loaded = false;
    std::chrono::steady_clock::time_point m_last_big_ad_showed{std::chrono::steady_clock::now() -
                                                               std::chrono::hours(1)};
    std::chrono::steady_clock::time_point m_last_big_ad_loaded{m_last_big_ad_showed};

    std::chrono::steady_clock::time_point m_last_rewarded_ad_showed{m_last_big_ad_showed};
    std::chrono::steady_clock::time_point m_last_rewarded_ad_loaded{m_last_big_ad_showed};

    void* m_ptr;
    bool taryfa_ulgowa = true;
    bool banner_showed = false;
};