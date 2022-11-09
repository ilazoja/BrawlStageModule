#pragma once

#include <memory.h>
#include <types.h>
#include <st/st_melee.h>
#include "st_qbert_stage_data.h"
#include "gr_qbert_background.h"
#include "gr_qbert_hud.h"
#include "gr_qbert_cube.h"
#include "gr_qbert_disk.h"
#include "gr_qbert_alien.h"
#include "gr_qbert_coily.h"
#include "gr_qbert_green.h"
#include "gr_qbert_red.h"
#include "gr_qbert_score.h"
#include <mt/mt_prng.h>

const float BGM_PLAY_OFFSET_FRAME = 0.0f;
const float BGM_VOLUME = 1.0f;
const float SCROLL_DIR = 0.0f;
const float POKETRAINER_Z = 0.0f;

#define NUM_BLOCKS 28
#define NUM_DISKS 14
#define NUM_ENEMIES 4
#define STARTING_COMPLETION_POINTS 1000
#define ADDED_POINTS_PER_ROUND 250
#define REMAINING_DISKS_POINTS 50

class stQbert : public stMelee {
    protected:
        snd3DGenerator soundGenerator;
        u8 numBlocksPerTeam[NUM_TEAMS];
        bool disksActive[NUM_DISKS];
        ImmobilizeState immobilizeState;
        float bgmTimer;
        float diskTimer;
        Vec3f scorePositions[NUM_PLAYERS*NUM_DIGITS];
        u32 teamScores[4];
public:
        stQbert(int stageID) : stMelee("stQbert", stageID) {
            numBlocksPerTeam[0] = NUM_BLOCKS;
            for (u8 team = 0; team < NUM_TEAMS; team++) {
                numBlocksPerTeam[team] = 0;
            }
            __memfill(disksActive, false, NUM_DISKS);
            immobilizeState = Immobilize_None;
            bgmTimer = 0;
            __memfill(scorePositions, 0, 6*4*12);
            __memfill(teamScores, 0, 4*4);

        };
        static stQbert* create();

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
        int getAIRange();
        bool isAdventureStage();
        int getPokeTrainerDrawLayer();
        float getPokeTrainerPositionZ();
        void getPokeTrainerPointData(int * unk, int unk2);

        virtual void createObj();
        virtual bool loading();
        virtual void update(float frameDiff);
        virtual int getFinalTechniqColor();
        virtual bool isBamperVector();
        virtual ~stQbert() {this->releaseArchive(); };
        virtual void notifyEventInfoGo();

        void createObjBg(int mdlIndex);
        void createObjHud(int mdlIndex);
        void createObjCube(int mdlIndex, int collIndex);
        void createObjDisk(int mdlIndex, int collIndex, int diskIndex);
        grQbertAlien* createObjAlien(int mdlIndex);
        void createObjCoily(int mdlIndex, grQbertAlien* enemyTarget);
        void createObjGreen(int mdlIndex);
        void createObjRed(int mdlIndex);
        void createObjScore(int mdlIndex, int player, int type);
        void updateCubes(float frameDelta);
        void updateDisks(float frameDelta);
        void updateEnemies(float frameDelta);
        void updateBgm(float frameDelta);
};

// TODO: Add option to make game slightly faster/harder after every round
