#pragma once

#include <st/st_melee.h>
#include "gr_qbert_alien.h"
#include "gr_qbert_cube.h"

#define HATCH_FRAMES 30.0
#define MIN_RESPAWN_TIME 600.0
#define MAX_RESPAWN_TIME 1200.0
#define COILY_EGG_HP 10.0
#define COILY_SNAKE_HP 20.0

class grQbertCoily : public grQbertEnemy
{
    protected:
        grQbertAlien* enemyTarget;
        float damage;
        bool isHatched;
        bool isDead;
    public:
        grQbertCoily(char* taskName) : grQbertEnemy(taskName) {
            isHatched = false;
            isDead = false;
            damage = 0;
        };
        virtual void onInflictEach(soCollisionLog* collisionLog, float power);
        virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
        virtual ~grQbertCoily() { };

        virtual void setupAttack();
        virtual void setupHitPoint();
        virtual void setStartPos();
        virtual void setTargetPos();
        virtual void setStart();
        virtual void updateMove(float frameDelta);
        virtual void setAnim();

        static grQbertCoily* create(int mdlIndex, char* tgtNodeName, char* taskName, stMelee* stage, grQbertAlien* enemyTarget);

};