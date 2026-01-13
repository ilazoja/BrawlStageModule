#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_bowsercastle_brick.h"
#include <mt/mt_prng.h>
#include <OS/OSError.h>

grBowserCastleBrick* grBowserCastleBrick::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastleBrick* brick = new(Heaps::StageInstance) grBowserCastleBrick(taskName);
    brick->setupMelee();
    brick->setMdlIndex(mdlIndex);
    brick->m_heapType = Heaps::StageInstance;
    brick->makeCalcuCallback(1, Heaps::StageInstance);
    brick->setCalcuCallbackRoot(7);

    return brick;
}

void grBowserCastleBrick::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);

    this->createEffectWork(2);
    SimpleEffectData simpleEffectData;
    this->createSimpleEffectData(&simpleEffectData, ef_ptc_common_landing_smoke, "Effect");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[0].m_id = simpleEffectData.m_id;
        this->m_effects[0].m_repeatFrame = simpleEffectData.m_repeatFrame;
        this->m_effects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
        this->m_effects[0].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[0].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[0].m_scale = 1.0;
    }
    this->createSimpleEffectData(&simpleEffectData, (EfID)0x2b, "Effect");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[1].m_id = simpleEffectData.m_id;
        this->m_effects[1].m_repeatFrame = simpleEffectData.m_repeatFrame;
        this->m_effects[1].m_nodeIndex = simpleEffectData.m_nodeIndex;
        this->m_effects[1].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[1].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[1].m_scale = 1.0;
    }

    this->createSoundWork(2,2);
    this->m_soundEffects[0].m_id = snd_se_stage_BowserCastle_Brick_Break;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_BowserCastle_Brick_Spawn;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);

    this->setupHitPoint();

    this->initializeEntity();
    this->startEntity();

    this->setHit();
    this->changeState(State_Spawn);

}

void grBowserCastleBrick::setupHitPoint() {
    Vec3f startOffsetPos = Vec3f(0,0,0);
    Vec3f endOffsetPos = Vec3f(0,0,0);
    this->setHitPoint(1.0, &startOffsetPos, &endOffsetPos, 1, this->getNodeIndex(0, "Hit"));
}

void grBowserCastleBrick::setHangCollision(grCollision* collision, int rightHangIndex, int leftHangIndex) {
    this->mainCollision = collision;
    this->rightHangIndex = rightHangIndex;
    this->leftHangIndex = leftHangIndex;
}

void grBowserCastleBrick::setHit() {
    // Done cause Brawl devs allocated ykData on the stack in grMadein::setupYakumonoClass leading to ykData being a garbage pointer so have to replace it in order to be able to change the HitSelfCategory
    this->m_yakumono->m_data = &this->yakumonoData;
    this->setSituationODD();
}

void grBowserCastleBrick::update(float deltaFrame){
    grMadein::update(deltaFrame);

    switch(this->state) {
        case State_Spawn:
            if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount()) {
                this->changeState(State_Passive);
            }
            break;
        case State_Break:
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_Spawn);
            }
            break;
        default:
            break;
    }

    Vec3f pos;
    this->getNodePosition(&pos, 0, "CollisionJoint");
    if (pos.m_z >= 0) {
        this->setCollisionStatus(true);
    }
    else {
        this->setCollisionStatus(false);
    }


}

void grBowserCastleBrick::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    this->changeState(State_Break);
    this->startGimmickEffect(0);
    this->startGimmickEffect(1);
    this->startGimmickSE(0);
}

void grBowserCastleBrick::changeState(State state) {
    stBowserCastleData* stageData = static_cast<stBowserCastleData*>(this->getStageData());
    this->setMotion(state);
    switch(state) {
        case State_Spawn:
            this->setSleepHit(true);
            this->m_soundEffects[1].m_generatorIndex = 1;
            this->startGimmickSE(1);
            break;
        case State_Passive:
            this->stopGimmickSE(1);
            this->setSleepHit(false);
            break;
        case State_Break:
            this->setSleepHit(true);
            this->timer = randi(stageData->brickMaxRespawnFrames - stageData->brickMinRespawnFrames) + stageData->brickMinRespawnFrames;
            break;
        default:
            break;

    }
    this->state = state;
}

void grBowserCastleBrick::setCollisionStatus(bool status) {
    this->setEnableCollisionStatus(status);
    if (this->mainCollision != NULL) {
        if (this->rightHangIndex >= 0) {
            this->mainCollision->getLine(this->rightHangIndex)->m_isHangR = !status;
        }
        if (this->leftHangIndex >= 0) {
            this->mainCollision->getLine(this->leftHangIndex)->m_isHangL = !status;
        }
    }
}