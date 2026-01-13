#pragma once

#include <gr/gr_madein.h>
#include <st/st_positions.h>
#include "st_ghosthouse_data.h"

class grGhostHouseBoo : public grMadein {

protected:
    enum State {
        State_Inactive = 0x0,
        State_Vanish = 0x1,
        State_Spawn = 0x2,
        State_StalkStart = 0x3,
        State_Stalk = 0x4,
        State_ShyStart = 0x5,
        State_Shy = 0x6,
        State_CircleStart = 0x7,
        State_Circle = 0x8,
        State_SnakeStart = 0x9,
        State_Snake = 0xA,
        State_CrewStart = 0xB,
        State_Crew = 0xC,
        State_ChaseStart = 0xD,
        State_Chase = 0xE,
        State_ChaseFinish = 0xF,
        State_AppearStart = 0x10,
        State_Appear = 0x11,
        State_Disappear = 0x12,
        State_Defeat = 0x13,
    };

    ykData yakumonoData;

    bool useAltAnim;

    int playerTarget;
    float speed;
    State state;
    float prevFollowAnimFrame;
    Vec3f prevPos;

    Vec2f circleCenterPos;
    float circleRadius;
    float circleCurrentAngle;

    Vec2f direction;
    grGhostHouseBoo* snakeLeader;
    grGhostHouseBoo* snakeTailgater;
    u8 numSnakeFollowers;
    float timer;

    float remainingDistance;
    float accel;
    Vec2f targetPos;

    Vec2f* southWestPos;
    Vec2f* northEastPos;

public:
    grGhostHouseBoo(const char* taskName) : grMadein(taskName), yakumonoData(1, NULL)
    {
        playerTarget = -1;
        speed = 0;
        state = State_Inactive;
        prevFollowAnimFrame = 0;
        snakeLeader = NULL;

    };
    virtual void update(float deltaFrame);
    virtual ~grGhostHouseBoo(){};
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

    virtual void setupAttack();
    virtual void setupHitPoint();
    virtual void setHit();

    virtual void setVanish();
    virtual void setSpawn(Rect2D* spawnRange, Vec3f* centerPos, bool useAltAnim);
    virtual void setStalk(int playerTarget);
    virtual void setCircle(grGimmickMotionPath* motionPath, float startRatio, float circleSpeed);
    virtual void setCircle(Vec2f* circleCenterPos, float circleRadius, float circleCurrentAngle, float circleAngleSpeed);
    virtual void setSnakeLeader(Rect2D* spawnRange, Vec3f* centerPos, u8 numFollowers);
    virtual void setSnakeFollow(grGhostHouseBoo* snakeLeader, float snakeTimer);
    virtual void setCrew(Vec2f* crewSWPos, Vec2f* crewNEPos);
    virtual void setChase(Vec3f* startPos, Vec2f* targetPos);
    virtual void setDisappear(Vec2f* disappearSWPos, Vec2f* disappearNEPos);
    virtual void setSnakeTailgater(grGhostHouseBoo* snakeTailgater);
    virtual float getMaxSnakeTimer();
    virtual void updateMove(float deltaFrame);
    virtual bool findClosestFighterDisp(Vec3f* outDisp);
    virtual void rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed);
    virtual void changeState(State state);

    static grGhostHouseBoo* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
