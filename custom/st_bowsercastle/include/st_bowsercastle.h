#pragma once

#include <types.h>
#include <st/st_melee.h>
#include <gr/gr_madein.h>
#include <ft/ft_manager.h>
#include "st_bowsercastle_data.h"
#include "gr_bowsercastle_ashiba.h"
#include "gr_bowsercastle_thwomp.h"
#include "gr_bowsercastle_brick.h"
#include "gr_bowsercastle_podoboo.h"
#include "gr_bowsercastle_background.h"

const float BGM_PLAY_OFFSET_FRAME = 0.0f;
const float BGM_VOLUME = 1.0f;
const float SCROLL_DIR = 0.0f;
const float POKETRAINER_Z = 0.0f;

class stBowserCastle : public stMelee, public ftOutsideEventObserver {
    protected:
        float laughTimer;
        float laughCooldownTimer;
        u8 numThwomps;
        bool isZoomedOut;

    public:
        stBowserCastle(srStageKind stageKind) : stMelee("stBowserCastle", stageKind), ftOutsideEventObserver(0) {
            addObserver(-1, -1);
            if (m_sendID < 0) {
                setupObserver(g_ftManager->m_eventManageModule.getManageId());
            }

            laughTimer = 0.0;
            laughCooldownTimer = 0.0;
            numThwomps = 0;
            isZoomedOut = false;

        };
        static stBowserCastle* create();

        virtual void createObj();
        virtual bool loading();
        virtual void update(float frameDiff);
        virtual GXColor getFinalTechniqColor();
        virtual bool isBamperVector();
        virtual ~stBowserCastle() {this->releaseArchive(); };

        virtual void notifyEventSuicide(int entryId);

        void createObjAshiba(int mdlIndex);
        void createObjBackground(int mdlIndex);
        void createObjThwomp(int mdlIndex, int collIndex, Vec2f* pos, int type, int shadowIndex, Vec2f* shadowPos);
        void createObjBrick(int mdlIndex, int collIndex, Vec2f* pos, int rightHangIndex, int leftHangIndex);
        void createObjPodoboo(int mdlIndex, Vec3f* pos);

};