#pragma once

#include <StaticAssert.h>
#include <st/st_melee.h>
#include <types.h>

const float BGM_PLAY_OFFSET_FRAME = 0.0f;
const float BGM_VOLUME = 1.0f;
const float SCROLL_DIR = 0.0f;
const float POKETRAINER_Z = 0.0f;

class stSkyTower : public stMelee {
public:
    stSkyTower() : stMelee("stSkyTower", Stages::Final){};
    static stSkyTower* create();
    grGimmickWindData2nd* getWind2ndOnlyData();
    bool isReStartSamePoint();
    int getPokeTrainerPointNum();
    void startFighterEvent();
    int initializeFighterAttackRatio();
    int helperStarWarp();
    bool isSimpleBossBattleMode();
    bool isBossBattleMode();
    bool isCameraLocked();
    void notifyTimmingGameStart();
    float getFrameRuleTime();
    void setFrameRuleTime(float newTime);
    bool isNextStepBgmEqualNowStepBgmFromFlag();
    float getBgmPlayOffsetFrame();
    float getBgmVolume();
    void setBgmChange(float unk, u32 unk1, u32 unk2);
    void getBgmChangeID(u32 unk1, float unk2);
    bool isBgmChange();
    int getBgmOptionID();
    int getNowStepBgmID();
    int getBgmID();
    int getBgmIDOverload();
    void appearanceFighterLocal();
    int getScrollDir(u32 unk1);
    int getDefaultLightSetIndex();
    Rect2D* getAIRange();
    bool isAdventureStage();
    int getPokeTrainerDrawLayer();
    float getPokeTrainerPositionZ();
    void getPokeTrainerPointData(int* unk, int unk2);

    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual int getFinalTechniqColor();
    virtual bool isBamperVector();
    virtual ~stSkyTower() {
        this->releaseArchive();
    };

};
static_assert(sizeof(stSkyTower) == 0x1D8, "Class is the wrong size!");