#pragma once

#include <StaticAssert.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <types.h>
#include <mt/mt_vector.h>
#include "gr_final.h"
#include "gr_targetsmash_target.h"
#include "gr_targetsmash_disk.h"
#include "gr_platform.h"
#include "gr_barrel_cannon.h"
#include "gr_spring.h"
#include "gr_ladder.h"
#include "gr_elevator.h"
#include "gr_punch_slider.h"
#include "gr_warpzone.h"
#include "gr_catapult.h"
#include <TRK/flushcache.h>

const float BGM_PLAY_OFFSET_FRAME = 0.0f;
const float BGM_VOLUME = 1.0f;
const float SCROLL_DIR = 0.0f;
const float POKETRAINER_Z = 0.0f;
const float UNK_FLOAT1 = 0.0f;

class stTargetSmash : public stMelee {
protected:
    gfArchive itemBrres;
    gfArchive itemParam;
    char _476[848 - 728];
    u32 level; // 848 (Required offset for stOperatorRuleTargetBreak!)
    u32 targetsHit; // 852 (Required offset for stOperatorRuleTargetBreak!)
    u32 targetsLeft; // 856 (Required offset for stOperatorRuleTargetBreak!)
    bool isItemsInitialized;
    char _860[912 - 861];
    float totalDamage; // 912 (Required offset for stOperatorRuleTargetBreak!)
    u32 numTargetsHitPerPlayer[NUM_PLAYERS]; // 916 (Required offset for stOperatorRuleTargetBreak!)

public:
    stTargetSmash() : stMelee("stTargetSmash", Stages::TBreak)
    {
        isItemsInitialized = false;
        targetsLeft = 0;
        targetsHit = 0;
        totalDamage = 0.0;
        __memfill(&numTargetsHitPerPlayer, 0, sizeof(numTargetsHitPerPlayer));
    };
    static stTargetSmash* create();
    int getWind2ndOnlyData();
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
    stRange* getAIRange();
    bool isAdventureStage();
    int getPokeTrainerDrawLayer();
    float getPokeTrainerPositionZ();
    void getPokeTrainerPointData(int* unk, int unk2);

    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual int getFinalTechniqColor();
    virtual bool isBamperVector();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, int itemID, int variantID);
    virtual ~stTargetSmash() { this->releaseArchive(); };

    void patchInstructions();
    void createObjAshiba(int mdlIndex);
    void createObjTarget(int mdlIndex, Vec2f* pos, Vec3f* scale, int motionPathIndex, int effectIndex, int collIndex);
    void createObjDisk(int mdlIndex, Vec2f* pos, float rot, float scaleX, float scaleZ, int motionPathIndex, int collIndex, int mode);
    void createObjPlatform(int mdlIndex, Vec2f* pos, float rot, float scale, int motionPathIndex, int collIndex);
    void createObjBreak(int mdlIndex, Vec2f* pos, float rot, int motionPathIndex, int collIndex, float maxDamage, float respawnTime);
    void createObjLand(int mdlIndex, Vec2f* pos, float rot, int motionPathIndex, int collIndex, float maxLandings, float respawnTime);
    void createObjPunchSlider(int mdlIndex, int sliderPathIndex, int motionPathIndex, float, float, float, float, float, float);
    void createObjElevator(int mdlIndex, Vec2f* pos, Vec2f* range, int collIndex, int posIndex, float speed, float deltaSpeed);
    void createObjSpring(int mdlIndex, int collIndex, Vec2f* pos, float rot, Vec2f* range, float bounce, int motionPathIndex);
    void createObjCannon(int mdlIndex, Vec2f* pos, float rot, float rotSpeed, float maxRot, int motionPathIndex, bool alwaysRotate, bool fullRotate, float autoFireFrames);
    void createObjLadder(int mdlIndex, Vec2f* pos, int motionPathIndex, bool restrictUpExit, bool);
    void createObjCatapult(int mdlIndex, float vector, float motionRatio, int motionPathIndex, float framesBeforeStartMove, float unk1, float unk2);
    void createObjWarpZone(int mdlIndex, Vec2f* pos, float rot, float scale, Vec2f* range, int motionPathIndex, float deactivateFrames, Vec2f* dest, u8 warpType, bool isNotAuto, int connectedMdlIndex, int connectedMotionPathIndex);
    void createTriggerHitPointEffect(Vec2f* posSW, Vec2f* posNE, float damage, short detectionRate);
    void createTriggerConveyor(Vec2f* posSW, Vec2f* posNE, float speed, bool isRightDirection);
    void createTriggerWater(Vec2f* posSW, Vec2f* posNE, float speed, bool canDrown);
    void createTriggerWind(Vec2f* posSW, Vec2f* posNE, float strength, float angle);
    void putItem(int itemID, u32 variantID, Vec3f* pos);

    STATIC_CHECK(sizeof(stTargetSmash) == 916 + NUM_PLAYERS*4)
};