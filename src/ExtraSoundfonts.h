#include <array>
#include <string_view>
#include <cstdint>

struct ExtraSoundfont
{
    std::string_view name;
    std::string_view author;
    std::string_view url;
    std::string_view hash;
    std::uint8_t mb_size;
};

inline constexpr std::array<ExtraSoundfont, 7> extra_soundfonts
{
    ExtraSoundfont{"Rhodes EVP73","OK73","https://musical-artifacts.com/artifacts/371/rhodes_73.sf2","d1e62b96a055806145fcc51e7e317f16332d661777cd39469304abf8dd66f4af",16},
    ExtraSoundfont{"Real Honky Tonk","Milton Paredes","https://musical-artifacts.com/artifacts/630/real_honky_tonk.sf2","ef7c012df6bafaab3f122d9a539c652f5dde1b1ad6db429747b4b1fa96e4fe1b",10},
    ExtraSoundfont{"Campbells Grand Piano Beta 2","Campbell Barton","https://musical-artifacts.com/artifacts/372/campbellspianobeta2.sf2","849321a3a7b73159e3140981f7049fe078e03f1ec8483c669c06c99a079097c4",6},
    ExtraSoundfont{"Yamaha P-45","Paw Inc.","https://musical-artifacts.com/artifacts/1599/YAMAHA_P_45_SOUNDBANK.sf2","d272fd075cf40b3b187fcad1eeeaccbc8cbb5306fbfa9163aacb883edf3c2d40",1},
    ExtraSoundfont{"Yamaha MX100II Disklavier","Sebastian Tomczak","https://musical-artifacts.com/artifacts/1351/Disklavier_from_little-scale.sf2","f5f303863234ed6888647b4412ff15ce718e6c499fb10642495074cf27520bfc",14},
    ExtraSoundfont{"Yamaha YPT 220","glassesglasses8","https://musical-artifacts.com/artifacts/2573/Yamaha_YPT_220_soundfont_studio_version.sf2","2736427fa44a1bc709dad73304254425601dcb26c4b252dc28841731d5f173cd",4},
    ExtraSoundfont{"Kawai ES100 Concert Grand","GD CRAFT","https://musical-artifacts.com/artifacts/2679/GD_CRAFT_-_Kawai_ES100_Concert_Grand__SL_.sf2","12648b978d01eabacf4d14ba88bc333857135dc74ac197b067551e9a36d9c73e",27}
};