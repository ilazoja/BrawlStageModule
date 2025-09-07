#include <memory.h>
#include <hkmath/hkMath.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include "st_bowsercastle.h"
#include <OS/OSError.h>

using namespace hkMath;

static stClassInfoImpl<Stages::Final, stBowserCastle> classInfo = stClassInfoImpl<Stages::Final, stBowserCastle>();

stBowserCastle* stBowserCastle::create() {
    stBowserCastle* stage = new(Heaps::StageInstance) stBowserCastle(Stages::Final);
    return stage;
}
bool stBowserCastle::loading(){
   return true;
}


void stBowserCastle::createObj() {
    this->testStageParamInit(this->m_fileData, 0xA);
    this->testStageDataInit(this->m_fileData, 0x14, 1);

    this->initCameraParam();
    nw4r::g3d::ResFile posData(m_fileData->getData(Data_Type_Model, 0x64, 0xfffe));
    if (posData.ptr() == NULL)
    {
        // if no stgPos model in pac, use defaults
        createStagePositions();
    }
    else
    {
        createStagePositions(&posData);
    }

    this->createObjAshiba(0);
    this->createObjBackground(1);

    this->createWind2ndOnly();
    this->loadStageAttrParam(this->m_fileData, 0x1E);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    this->registScnAnim(scnData, 0);
    this->initPosPokeTrainer(1, 0);
    this->createObjPokeTrainer(this->m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);

    stBowserCastleData* stageData = static_cast<stBowserCastleData*>(this->m_stageData);
    this->setStageAttackData(&stageData->damageFloor, 0);

}

void stBowserCastle::createObjAshiba(int mdlIndex) {
    grBowserCastleAshiba* ground = grBowserCastleAshiba::create(mdlIndex, "", "grBowserCastleStage");
    if(ground != NULL){
        this->addGround(ground);
        ground->startup(m_fileData,0,0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
        this->createCollision(this->m_fileData, 2, ground);

        u32 thwompsIndex = ground->getNodeIndex(0, "Thwomps");
        u32 bricksIndex = ground->getNodeIndex(0, "Bricks");
        u32 podoboosIndex = ground->getNodeIndex(0, "Podoboos");
        u32 endIndex = ground->getNodeIndex(0, "End");

        for (int i = thwompsIndex + 1; i < bricksIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjThwomp(resNodeData->m_rotation.m_x, resNodeData->m_rotation.m_y, resNodeData->m_translation.xy(), resNodeData->m_rotation.m_z, resNodeData->m_scale.m_z, resNodeData->m_scale.xy());
            this->numThwomps++;
        }
        for (int i = bricksIndex + 1; i < podoboosIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjBrick(resNodeData->m_rotation.m_x, resNodeData->m_rotation.m_y, resNodeData->m_translation.xy(), resNodeData->m_scale.m_y, resNodeData->m_scale.m_z);
        }
        for (int i = podoboosIndex + 1; i < endIndex; i++) {
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(i).ptr();
            this->createObjPodoboo(resNodeData->m_rotation.m_x, &resNodeData->m_translation);
        }

    }
}

void stBowserCastle::createObjBackground(int mdlIndex) {
    grBowserCastleBackground* ground = grBowserCastleBackground::create(mdlIndex, "", "grBowserCastleBackground");
    if(ground != NULL) {
        this->addGround(ground);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stBowserCastle::createObjThwomp(int mdlIndex, int collIndex, Vec2f* pos, int type, int shadowMdlIndex, Vec2f* shadowPos) {
    grBowserCastleThwomp* thwomp = grBowserCastleThwomp::create(mdlIndex, "", "grBowserCastleThwomp");
    if(thwomp != NULL){
        this->addGround(thwomp);
        thwomp->setStageData(m_stageData);
        thwomp->startup(m_fileData,0,0);
        if (collIndex > 0) {
            createCollision(m_fileData, collIndex, thwomp);
        }
        thwomp->setPos(pos->m_x, pos->m_y, 0);

        grBowserCastleShadow* shadow = grBowserCastleShadow::create(shadowMdlIndex, "", "grBowserCastleShadow");
        if (shadow != NULL) {
            this->addGround(shadow);
            shadow->setStageData(m_stageData);
            shadow->startup(m_fileData,0,0);
            shadow->setPos(shadowPos->m_x, shadowPos->m_y, 0);
            shadow->changeState(grBowserCastleShadow::State_Break);
            thwomp->setShadow(shadow);
            thwomp->setType(type);
        }
    }
}

void stBowserCastle::createObjBrick(int mdlIndex, int collIndex, Vec2f* pos, int rightHangIndex, int leftHangIndex) {
    grBowserCastleBrick* ground = grBowserCastleBrick::create(mdlIndex, "", "grBowserCastleBrick");
    if(ground != NULL){
        this->addGround(ground);
        ground->setStageData(m_stageData);
        ground->startup(m_fileData,0,0);
        if (collIndex > 0) {
            createCollision(m_fileData, collIndex, ground);
            grCollision* collision = this->getGround(0)->m_collision;
            if (collision != NULL) {
                ground->setHangCollision(collision, rightHangIndex, leftHangIndex);
            }
        }
        ground->setPos(pos->m_x, pos->m_y, 0);

    }
}

void stBowserCastle::createObjPodoboo(int mdlIndex, Vec3f* pos) {
    grBowserCastlePodoboo* podoboo = grBowserCastlePodoboo::create(mdlIndex, "", "grBowserCastlePodoboo");
    if(podoboo != NULL){
        this->addGround(podoboo);
        podoboo->setStageData(m_stageData);
        podoboo->startup(m_fileData,0,0);
        podoboo->setPos(pos);

    }
}

void stBowserCastle::update(float deltaFrame){
    stBowserCastleData* castleData = static_cast<stBowserCastleData*>(this->m_stageData);

    bool isThwompSpawning = false;
    for (u8 i = 0; i < this->numThwomps; i++) {
        grBowserCastleThwomp* thwomp = static_cast<grBowserCastleThwomp*>(this->getGround(2*i + 1));
        if (thwomp->getState() == grBowserCastleThwomp::State_Spawn) {
            if (!this->isZoomedOut && castleData->thwompSpawnZoomOut.m_x >= 0 && castleData->thwompSpawnZoomOut.m_y >= 0) {
                this->zoomOutCamera(castleData->thwompSpawnZoomOut.m_x, castleData->thwompSpawnZoomOut.m_y);
                this->isZoomedOut = true;
            }
            isThwompSpawning = true;
        }
    }
    if (!isThwompSpawning && this->isZoomedOut) {
        this->zoomInCamera();
        this->isZoomedOut = false;
    }

    if (this->laughTimer > 0) {
        this->laughTimer -= deltaFrame;
        if (this->laughTimer <= 0) {
            this->playSeBasic(snd_se_stage_BowserCastle_Laugh,0);
            this->laughCooldownTimer = castleData->laughCooldownFrames;
        }
    }
    else if (this->laughCooldownTimer > 0) {
        this->laughCooldownTimer -= deltaFrame;
    }

}

void stBowserCastle::notifyEventSuicide(int entryId) {
    stBowserCastleData* castleData = static_cast<stBowserCastleData*>(this->m_stageData);

    if (this->laughTimer <= 0 && this->laughCooldownTimer <= 0) {
        this->laughTimer = castleData->laughStartFrames;
    }
}

bool stBowserCastle::isBamperVector() {
   return true;
}
GXColor stBowserCastle::getFinalTechniqColor() {
   return (GXColor){0x14000496};
}

template<Stages::srStageKind I, typename T>
T* stClassInfoImpl<I, T>::create(){
    T* stage = new(Heaps::StageInstance) T(I);
    return stage;
}

template<Stages::srStageKind I, typename T>
stClassInfoImpl<I, T>::~stClassInfoImpl() {
   setClassInfo(I,0);
}

template<Stages::srStageKind I, typename T>
void stClassInfoImpl<I, T>::preload() {
   return;
}

