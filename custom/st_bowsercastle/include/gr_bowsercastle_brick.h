#pragma once

#include <gr/gr_madein.h>
#include "st_bowsercastle_data.h"

class grBowserCastleBrick : public grMadein
{
    protected:
        enum State {
            State_Spawn = 0x0,
            State_Passive = 0x1,
            State_Break = 0x2,
        };

        ykData yakumonoData;
        State state;
        float timer;
        grCollision* mainCollision;
        int rightHangIndex;
        int leftHangIndex;

public:
        grBowserCastleBrick(const char* taskName) : grMadein(taskName), yakumonoData(1, NULL)  {
            mainCollision = NULL;
            rightHangIndex = -1;
            leftHangIndex = -1;
        };
        virtual void update(float frameDiff);
        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
        virtual ~grBowserCastleBrick() { };
        virtual void setupHitPoint();
        virtual void setHit();
        virtual void setHangCollision(grCollision* collision, int rightHangIndex, int leftHangIndex);
        virtual void setCollisionStatus(bool);
        virtual void changeState(State state);

    static grBowserCastleBrick* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
