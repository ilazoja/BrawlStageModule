#pragma once

#include <gr/gr_madein.h>
#include "st_bowsercastle_data.h"
#include "gr_bowsercastle_shadow.h"

class grBowserCastleThwomp : public grMadein
{
    public:
        enum State {
            State_Inactive = -0x1,
            State_Spawn = 0x0,
            State_Sleep = 0x1,
            State_Awake = 0x2,
            State_Shake = 0x3,
            State_Stomp = 0x4,
        };

        enum Type {
            Type_Left = 0x0,
            Type_Right = 0x1,
        };

    protected:

        soAreaData areaData;
        soSet<soAreaData> areaInit;
        ykAreaData areaInfo;
        State state;
        float timer;
        bool isPlayerUnder;
        Type type : 8;
        grBowserCastleShadow* shadow;
        int dangerZoneId;

    public:
        grBowserCastleThwomp(const char* taskName) : grMadein(taskName) {
            isPlayerUnder = false;
            shadow = NULL;
            dangerZoneId = -1;
        }
        virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
        virtual void update(float frameDiff);
        virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
        virtual void onInflictEach(soCollisionLog* collisionLog, float power);
        virtual void setShadow(grBowserCastleShadow* shadow);
        virtual void setType(u8 type);
        virtual void setupAttack();
        virtual void changeState(State state);
        virtual State getState();
        virtual ~grBowserCastleThwomp() { };

        static grBowserCastleThwomp* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
