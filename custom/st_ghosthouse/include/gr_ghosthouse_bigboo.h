#pragma once

#include <gr/gr_madein.h>
#include <st/st_positions.h>
#include "st_ghosthouse_data.h"

class grGhostHouseBigBoo : public grMadein {
protected:
    enum State {
        State_Inactive = 0x0,
        State_Spawn = 0x1,
        State_Vanish = 0x2,
        State_StalkStart = 0x3,
        State_Stalk = 0x4,
        State_ShyStart = 0x5,
        State_Shy = 0x6,
        State_HideStart = 0x7,
        State_Hide = 0x8,
        State_HideEnd = 0x9,
        State_Defeat = 0xA
    };

    ykData yakumonoData;

    State state;
    u8 numHits;

    float speed;
    float prevFollowAnimFrame;
    Vec3f prevPos;
    float timer;

    Vec2f* southWestPos;
    Vec2f* northEastPos;

    Vec3f hideDestPos;

public:
    grGhostHouseBigBoo(const char* taskName) : grMadein(taskName), yakumonoData(1, NULL)
    {
        state = State_Inactive;
        numHits = 0;
        speed = 0;
        prevFollowAnimFrame = 0;
    };
    virtual void update(float deltaFrame);
    virtual ~grGhostHouseBigBoo(){};
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

    virtual void setupAttack();
    virtual void setupHitPoint();
    virtual void setHit();

    virtual void setSpawn();
    virtual void setVanish();
    virtual void setStalk(Vec2f* crewSWPos, Vec2f* crewNEPos);
    virtual void changeState(State state);
    virtual void updateMove(float deltaFrame);

    virtual void rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed);
    virtual Vec3f getRandPos();

    static grGhostHouseBigBoo* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};

// TODO: Big Boo
// When hurt will travel to random points on map until cooldown
// After certain amount of hits, he will get angry and chase without getting shy