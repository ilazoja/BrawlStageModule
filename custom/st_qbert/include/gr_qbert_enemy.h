#pragma once

#include <st/st_melee.h>
#include <gr/gr_madein.h>
#include <gm/gm_global.h>
#include "gr_qbert_cube.h"
#include "st_qbert_stage_data.h"

#define STARTING_CUBE_INDEX 2

float distance(Vec3f* p1, Vec3f* p2);

class grQbertEnemy : public grMadein
{
protected:
    stMelee* stage;
    snd3DGenerator soundGenerator;
    float timer;
    float angle;
    float velocity;
    float shakeTimer;
    Vec3f shakeOffset;
    u8 targetIndex;
    bool isStart;
    float immobilizeTimer;
    u32* teamScoresWork;
    GameRule gameRule;
public:
    Vec3f prevPos;
    Vec3f targetPos;
    Vec3f midpointPos;
    bool isDead;

    grQbertEnemy(const char* taskName) : grMadein(taskName) {
        timer = 0;
        angle = 0;
        velocity = 0;
        shakeTimer = 0;
        shakeOffset = Vec3f(0, 0, 0);
        isStart = false;
        immobilizeTimer = 0;
        isDead = false;
    };
    virtual ~grQbertEnemy() { };
    virtual void update(float frameDelta);

    virtual void setupAttack() {};
    virtual void setupHitPoint() {};
    virtual void setStartPos() {};
    virtual void setTargetPos() {};
    virtual void setTeamScoresWork(u32* teamScoresWork, GameRule gamerule);
    virtual void setStart();
    virtual void setAnim();
    virtual void updateMove(float frameDelta) {};
    virtual void updateShake(float frameDelta);
    virtual void setImmobilize(float immobilizeDuration);
};