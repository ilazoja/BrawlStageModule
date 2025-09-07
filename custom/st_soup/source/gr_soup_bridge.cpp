#include <memory.h>
#include "gr_soup_bridge.h"
#include <OS/OSError.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>

grSoupBridge* grSoupBridge::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grSoupBridge* bridge = new(Heaps::StageInstance) grSoupBridge(taskName);
    bridge->setMdlIndex(mdlIndex);
    bridge->m_heapType = Heaps::StageInstance;
    bridge->makeCalcuCallback(1, Heaps::StageInstance);
    bridge->setCalcuCallbackRoot(7);

    bridge->setupHitPoint();
    return bridge;
}

void grSoupBridge::setupHitPoint() {
    Vec3f startOffsetPos = Vec3f(-20.0,0,0);
    Vec3f endOffsetPos = Vec3f(25.0,0,0);
    this->setHitPoint(2.0, &startOffsetPos, &endOffsetPos, 1, 1);
}

void grSoupBridge::setHit() {
    // Done cause Brawl devs allocated ykData on the stack in grMadein::setupYakumonoClass leading to ykData being a garbage pointer so have to replace it in order to be able to change the HitSelfCategory
    this->m_yakumono->m_data = &this->yakumonoData;
    this->m_yakumono->setCollisionHitSelfCatagory(soCollision::Category_Floor); // Changed category so that projectiles like Mario fireball don't get absorbed by it
}

void grSoupBridge::update(float deltaFrame) {
    this->updateBreak(deltaFrame);
    this->updateShake(deltaFrame);
}

void grSoupBridge::updateBreak(float deltaFrame) {
    if (this->breakTimer > 0) {
        this->breakTimer -= deltaFrame;
        if (this->breakTimer <= 0) {
            switch(this->bridgeState) {
                case BRIDGE_STATE_BROKE:
                    this->bridgeState = BRIDGE_STATE_BUILD;
                    this->breakTimer = REBUILD_FRAMES;
                    this->setEnableCollisionStatus(true);
                    this->setMotionDetails(0, 1, 0, 0, 0);
                    break;
                case BRIDGE_STATE_BUILD:
                    this->setMotionDetails(0, 0, 0, 0, 0);
                    this->setSleepHit(false);
                    break;
                default:
                    break;
            }
        }
    }
}

void grSoupBridge::updateShake(float frameDelta) {
    Vec3f shakeOffset = Vec3f(0, 0, 0);
    this->shakeTimer -= frameDelta;
    if (this->shakeTimer <= 0) {
        this->shakeTimer = 0;
    }
    else {
        if ((u32)this->shakeTimer % 3 == 0) {
            float x;
            float y;
            mtSinCosf(0, &y, &x);
            float shakeMul = 0.5 + 0.8*randf();
            shakeOffset = Vec3f(shakeMul*x, shakeMul*y, 0);
        }
    }
    this->setPos(&shakeOffset);
}

void grSoupBridge::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (damage->m_damage >= BRIDGE_HP) {
        damage->m_damage = 0;
        this->setMotionDetails(1, 0, 0, 0, 0);
        this->setEnableCollisionStatus(false);
        this->setSleepHit(true);
        this->breakTimer = BROKE_FRAMES;
        this->bridgeState = BRIDGE_STATE_BROKE;
        this->soundGenerator.playSE(snd_se_stage_Greenhill_landslide, 0x0, 0x0, 0xffffffff);
    }
}
