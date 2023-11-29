#include <memory.h>
#include <st/st_class_info.h>
#include "st_ghosthouse.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <hk/hk_math.h>
#include <mt/mt_prng.h>
#include <stdio.h>
#include <it/it_manager.h>
#include <OS/OSError.h>

static stClassInfoImpl<Stages::Final, stGhostHouse> classInfo = stClassInfoImpl<Stages::Final, stGhostHouse>();

stGhostHouse* stGhostHouse::create() {
    stGhostHouse* stage = new(Heaps::StageInstance) stGhostHouse(Stages::Final);
    return stage;
}

bool stGhostHouse::loading(){
    return true;
}

void stGhostHouse::createObj() {

    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);
    initCameraParam();

    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(m_stageData);

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

    createObjGround(1);
    createObjGround(2);
    u32 groundCount = 2;
    this->booStartGroundIndex = groundCount;
    for (int i = 0; i < ghostHouseData->numEachBoos; i++) {
        createObjBoo(3, false);
        groundCount++;
    }
    for (int i = 0; i < ghostHouseData->numEachBoos; i++) {
        createObjBoo(4, i % 2);
        groundCount++;
    }
    for (int i = 0; i < ghostHouseData->numEachBoos; i++) {
        createObjBoo(5, false);
        groundCount++;
    }
    for (int i = 0; i < ghostHouseData->numEachBoos; i++) {
        createObjBoo(6, false);
        groundCount++;
    }
    this->bubbleStartGroundIndex = groundCount;
    for (int i = 0; i < ghostHouseData->numBubbles; i++) {
        createObjBubble(11);
        groundCount++;
    }
    this->eerieStartGroundIndex = groundCount;
    for (int i = 0; i < ghostHouseData->numEeries; i++) {
        createObjEerie(12);
        groundCount++;
    }
    this->fishingBooStartGroundIndex = groundCount;
    for (int i = 0; i < ghostHouseData->numFishingBoos; i++) {
        createObjGround(1);
        groundCount++;
    }
    this->circleMotionPathStartGroundIndex = groundCount;

    grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
    this->numPlayerCircles = ground->getNumNodesWithFormat("PCircle%d");
    this->numSetCircles = ground->getNumNodesWithFormat("Circle%d");
    this->numEerieFormations = ground->getNumNodesWithFormat("Eeries%d");

    for (int i = 0; i < this->numSetCircles; i++) {
        u32 nodeIndex;
        ground->getNodeIndexWithFormat(&nodeIndex, 0, "Circle%d", i);
        nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex)).ptr();

        if (int(resNodeData->m_translation.m_z) > 0) {
            for (int j = 0; j < int(resNodeData->m_rotation.m_x); j++) {
                this->createObjMotionPath(resNodeData->m_translation.m_z, j);
            }
        }

    }

    createCollision(m_fileData, 2, NULL);

    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 30);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_unk, 0x0);
}

void stGhostHouse::createObjGround(int mdlIndex) {
    grGhostHouse* ground = grGhostHouse::create(mdlIndex, "", "grGhostHouse");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stGhostHouse::createObjBoo(int mdlIndex, bool useAltAnim) {
    grGhostHouseBoo* boo = grGhostHouseBoo::create(mdlIndex, "", "grGhostHouseBoo");
    if (boo != NULL)
    {
        addGround(boo);
        boo->setStageData(m_stageData);
        boo->startup(m_fileData, 0, 0);
        boo->setupAttack();
        boo->initializeEntity();
        boo->startEntity();
        boo->setSpawn(&this->m_cameraParam1->m_range, &this->m_cameraParam1->m_centerPos, useAltAnim);
    }
}
void stGhostHouse::createObjBubble(int mdlIndex) {
    grGhostHouseBubble* bubble = grGhostHouseBubble::create(mdlIndex, "", "grGhostHouseBubble");
    if (bubble != NULL)
    {
        addGround(bubble);
        bubble->setStageData(m_stageData);
        bubble->startup(m_fileData, 0, 0);
        bubble->setupAttack();
        bubble->initializeEntity();
        bubble->startEntity();
        bubble->setVanish();
    }
}

void stGhostHouse::createObjEerie(int mdlIndex) {
    grGhostHouseEerie* eerie = grGhostHouseEerie::create(mdlIndex, "", "grGhostHouseEerie");
    if (eerie != NULL)
    {
        addGround(eerie);
        eerie->setStageData(m_stageData);
        eerie->startup(m_fileData, 0, 0);
        eerie->setupAttack();
        eerie->initializeEntity();
        eerie->startEntity();
        eerie->setVanish();
    }
}

void stGhostHouse::createObjMotionPath(int mdlIndex, int index) {
    char nodeName[32];
    sprintf(nodeName, "Boo%d", index);
    const char* nodeNamePtr = nodeName;
    grGimmickMotionPath* motionPath = grGimmickMotionPath::create(mdlIndex, nodeNamePtr, "grGimmickMotionPath");
    if (motionPath != NULL)
    {
        addGround(motionPath);
        motionPath->setGimmickData(&this->circleMotionPathData);
        motionPath->startup(m_fileData, 0, 0);
    }
}

void stGhostHouse::notifyEventInfoGo() {
    this->changeEvent(this->decideNextEvent());
}

void stGhostHouse::update(float deltaFrame){
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            ipButton currentButton = fighter->m_moduleAccesser->getControllerModule()->getButton();
            if (currentButton.m_downTaunt) {
                this->changeEvent(Event_Stalk);
            }
            else if (currentButton.m_leftTaunt) {
                this->changeEvent(Event_Circle);
            }
            else if (currentButton.m_rightTaunt) {
                this->changeEvent(Event_Eerie);
            }
            else if (currentButton.m_upTaunt) {
                this->changeEvent(Event_Crew);
            }
        }
    }

    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(m_stageData);
    switch(this->currentEvent) {
        case Event_Eerie:
        {
            this->eerieTimer -= deltaFrame;
            if (this->eerieTimer <= 0) {
                grGhostHouseEerie* availableEeries[MAX_NUM_ENEMY];
                u32 numAvailableEeries = 0;
                for (int i = 0; i < ghostHouseData->numEeries; i++) {
                    grGhostHouseEerie* eerie = static_cast<grGhostHouseEerie*>(this->getGround(this->eerieStartGroundIndex + i));
                    if (eerie->isAvailable()) {
                        availableEeries[numAvailableEeries] = eerie;
                        numAvailableEeries++;
                    }
                }

                u32 chosenFormation = randi(this->numEerieFormations);

                grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
                u32 nodeIndex = ground->getNodeIndex(0, "Eeries");
                nw4r::g3d::ResNodeData* resNodeDataSW = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
                nw4r::g3d::ResNodeData* resNodeDataNE = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 2)).ptr();

                ground->getNodeIndexWithFormat(&nodeIndex, 0, "Eeries%d", chosenFormation);

                nw4r::g3d::ResNodeData* eerieResNode = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
                int numEeries = eerieResNode->m_rotation.m_x;
                float directionX = -1.0f + 2 * randi(2);
                float startPosY = randf()*(resNodeDataNE->m_translation.m_y - resNodeDataSW->m_translation.m_y) + resNodeDataSW->m_translation.m_y;

                for (int i = 0; i < numEeries; i++) {
                    if (i < numAvailableEeries) {
                        grGhostHouseEerie* eerie = availableEeries[i];

                        eerieResNode = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1 + i)).ptr();
                        Vec3f startPos;
                        if (directionX > 0) {
                            startPos = (Vec3f){resNodeDataSW->m_translation.m_x - eerieResNode->m_translation.m_x, startPosY + eerieResNode->m_translation.m_y, 0.0};
                        } else {
                            startPos = (Vec3f){resNodeDataNE->m_translation.m_x + eerieResNode->m_translation.m_x, startPosY + eerieResNode->m_translation.m_y, 0.0};
                        }

                        float period = eerie->getPeriod();
                        float startAnimFrame = eerieResNode->m_rotation.m_y + randi(period);
                        if (startAnimFrame >= period) {
                            startAnimFrame -= period;
                        }

                        eerie->setActive(&resNodeDataSW->m_translation.m_xy, &resNodeDataNE->m_translation.m_xy,
                                         &startPos, directionX, eerieResNode->m_rotation.m_z,
                                         startAnimFrame);
                    }


                }

                this->eerieTimer = randi(ghostHouseData->eerieWaitMaxFrames - ghostHouseData->eerieWaitMinFrames) + ghostHouseData->eerieWaitMinFrames;


            }
        }
            break;
        default:
            break;
    }

    if (this->eventStartTimer > 0.0) {
        this->eventStartTimer -= deltaFrame;
        if (this->eventStartTimer <= 0.0) {
            this->startNextEvent();
        }
    }
}

stGhostHouse::GhostEvent stGhostHouse::decideNextEvent() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(m_stageData);

    float totalChance = 0.0;
    for (int i = 0; i < NUM_EVENTS; i++) {
        if (i != this->currentEvent) {
            totalChance += ghostHouseData->eventChances[i];
        }
    }
    float targetChance = totalChance*randf();

    float currentChanceSum = 0.0;
    for (int i = 0; i < NUM_EVENTS; i++) {
        if (i != this->currentEvent) {
            currentChanceSum += ghostHouseData->eventChances[i];
            if (currentChanceSum >= totalChance) {
                return static_cast<GhostEvent>(i);
            }
        }
    }
    return Event_None;
}

void stGhostHouse::startNextEvent() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(m_stageData);

    this->currentEvent = this->nextEvent;
    switch (this->currentEvent) {
        case Event_Stalk:
        {
            int numBoosToFollow = randi(ghostHouseData->maxNumBoosToFollow - ghostHouseData->minNumBoosToFollow) +
                                  ghostHouseData->minNumBoosToFollow;
            numBoosToFollow =
                    hkMath::min2<int>(g_ftManager->getEntryCount() * numBoosToFollow, ghostHouseData->numEachBoos) /
                    g_ftManager->getEntryCount();

            for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
                int entryId = g_ftManager->getEntryIdFromIndex(i);
                if (g_ftManager->isFighterActivate(entryId, -1)) {
                    for (int j = 0; j < numBoosToFollow; j++) {
                        grGhostHouseBoo *boo = static_cast<grGhostHouseBoo *>(this->getGround(
                                this->booStartGroundIndex + i * numBoosToFollow + j));
                        boo->setStalk(g_ftManager->getPlayerNo(entryId));
                    }
                }
            }
        }
            break;
        case Event_Circle:
        {
            grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));

            u32 numCircles = this->numSetCircles;

            int entryIds[NUM_PLAYERS];
            u32 numPlayersOnGround = 0;
            if (ghostHouseData->booCirclePlayerProbability > randf()) {
                for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
                    int entryId = g_ftManager->getEntryIdFromIndex(i);
                    if (g_ftManager->isFighterActivate(entryId, -1)) {
                        if (soExternalValueAccesser::getSituationKind(g_ftManager->getFighter(entryId, -1)) == 0) {
                            entryIds[numPlayersOnGround] = entryId;
                            numPlayersOnGround++;
                        }
                    }
                }

                if (numPlayersOnGround > 0) {
                    numCircles = this->numPlayerCircles;
                }
            }

            u32 chosenCircle = randi(numCircles);
            u32 groundCount = this->circleMotionPathStartGroundIndex;

            float startRatio = randf();
            int speedMultiplier = -1 + 2*randi(2);

            for (int i = 0; i < numCircles; i++) {
                u32 nodeIndex;
                if (numPlayersOnGround > 0) {
                    ground->getNodeIndexWithFormat(&nodeIndex, 0, "PCircle%d", i);
                }
                else {
                    ground->getNodeIndexWithFormat(&nodeIndex, 0, "Circle%d", i);
                }
                nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex)).ptr();
                if (int(resNodeData->m_translation.m_z) > 0) {
                    if (chosenCircle == i) {
                        int numBoos = resNodeData->m_rotation.m_x;
                        for (int j = 0; j < numBoos; j++) {
                            grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + ghostHouseData->numEachBoos*(j % 4) + j/4));
                            grGimmickMotionPath* motionPath = static_cast<grGimmickMotionPath*>(this->getGround(groundCount + j));
                            boo->setCircle(motionPath, startRatio, speedMultiplier);
                        }
                        break;
                    }
                    groundCount += resNodeData->m_rotation.m_x;
                }
                else {
                    if (chosenCircle == i) {
                        Vec2f centerCircle = resNodeData->m_translation.m_xy;
                        Vec2f center = centerCircle;
                        if (numPlayersOnGround > 0) {
                            center = g_ftManager->getFighterCenterPos(entryIds[randi(numPlayersOnGround)], -1).m_xy;
                        }

                        if (int(resNodeData->m_rotation.m_x) > 0) {
                            int numBoos = resNodeData->m_rotation.m_x;
                            for (int j = 0; j < numBoos; j++) {
                                grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + ghostHouseData->numEachBoos*(j % 4) + j/4));

                                u32 numBooSpots = resNodeData->m_rotation.m_x + resNodeData->m_rotation.m_y;
                                float angle = 2*M_PI*startRatio + 2*M_PI*j/numBooSpots;
                                float radius = resNodeData->m_scale.m_z;
                                float speed = speedMultiplier*resNodeData->m_rotation.m_z;
                                boo->setCircle(&center, radius, angle, speed);
                            }
                        }
                        else {
                            resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
                            float speed = speedMultiplier*resNodeData->m_rotation.m_z;
                            int numBoos = resNodeData->m_rotation.m_x;
                            for (int j = 0; j < numBoos; j++) {
                                grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + ghostHouseData->numEachBoos*(j % 4) + j/4));

                                resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1 + j)).ptr();
                                float radius = centerCircle.distance(&resNodeData->m_translation.m_xy);
                                float angle = 2*M_PI*startRatio + atan2(resNodeData->m_translation.m_y, resNodeData->m_translation.m_x);
                                boo->setCircle(&center, radius, angle, speed);
                            }
                        }
                    }
                }


            }
        }
            break;
        case Event_Snake:
        {
            int booIndex = 0;
            int numSnakes = randi(ghostHouseData->maxNumBooSnakes - ghostHouseData->minNumBooSnakes) + ghostHouseData->minNumBooSnakes;
            for (int i = 0; i < numSnakes; i++) {
                grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + booIndex));
                boo->setSnakeLeader(&this->m_cameraParam1->m_range, &this->m_cameraParam1->m_centerPos);
                int numBoosInSnake = randi(ghostHouseData->maxNumBoosInSnake - ghostHouseData->minNumBoosInSnake) + ghostHouseData->minNumBoosInSnake;

                for (int j = 1; j < numBoosInSnake; j++) {
                    if (j % 4 == 0) {
                        booIndex++;
                    }
                    if (booIndex < ghostHouseData->numEachBoos) {
                        grGhostHouseBoo* followerBoo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + ghostHouseData->numEachBoos*(j % 4) + booIndex));
                        Vec3f scale;
                        followerBoo->getNodeScale(&scale, 0, "Hurt");
                        float maxSnakeTimer = 2*scale.m_x*(numBoosInSnake-1)/ghostHouseData->booSnakeSpeed;

                        followerBoo->setSnakeFollow(boo, maxSnakeTimer, j*maxSnakeTimer/(numBoosInSnake-1));
                    }
                }
                booIndex++;
            }
        }
            break;
        case Event_Crew:
        {
            grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
            u32 nodeIndex = ground->getNodeIndex(0, "Crew");
            nw4r::g3d::ResNodeData* resNodeDataSW = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
            nw4r::g3d::ResNodeData* resNodeDataNE = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 2)).ptr();

            for (int i = 0; i < ghostHouseData->numEachBoos*4; i++) {
                grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + i));
                boo->setCrew(&resNodeDataSW->m_translation.m_xy, &resNodeDataNE->m_translation.m_xy);
            }
        }
            break;
        case Event_Disappear:
        {
            grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
            u32 nodeIndex = ground->getNodeIndex(0, "Disappear");
            nw4r::g3d::ResNodeData* resNodeDataSW = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
            nw4r::g3d::ResNodeData* resNodeDataNE = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 2)).ptr();

            for (int i = 0; i < ghostHouseData->numEachBoos*4; i++) {
                grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + i));
                boo->setDisappear(&resNodeDataSW->m_translation.m_xy, &resNodeDataNE->m_translation.m_xy);
            }
        }
            break;
        case Event_Bubble:
        {
            grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
            u32 nodeIndex = ground->getNodeIndex(0, "Bubbles");
            nw4r::g3d::ResNodeData* resNodeDataSW = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
            nw4r::g3d::ResNodeData* resNodeDataNE = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 2)).ptr();

            for (int i = 0; i < ghostHouseData->numBubbles; i++) {
                grGhostHouseBubble* bubble = static_cast<grGhostHouseBubble*>(this->getGround(this->bubbleStartGroundIndex + i));
                bubble->setActive(&resNodeDataSW->m_translation.m_xy, &resNodeDataNE->m_translation.m_xy);
            }
        }
            break;
        case Event_Eerie:
            this->eerieTimer = 0;
            break;
        default:
            break;
    }
}

void stGhostHouse::changeEvent(GhostEvent event) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(m_stageData);

    if (this->currentEvent != event && this->nextEvent != event) {
        switch (this->currentEvent) {
            case Event_Circle:
            case Event_Snake:
            case Event_Crew:
            case Event_Disappear:
            case Event_Stalk:
                for (int i = 0; i < ghostHouseData->numEachBoos*4; i++) {
                    grGhostHouseBoo* boo = static_cast<grGhostHouseBoo*>(this->getGround(this->booStartGroundIndex + i));
                    boo->setVanish();
                }
                this->eventStartTimer = 120;
                break;
            case Event_Bubble:
            {
                for (int i = 0; i < ghostHouseData->numBubbles; i++) {
                    grGhostHouseBubble* bubble = static_cast<grGhostHouseBubble*>(this->getGround(this->bubbleStartGroundIndex + i));
                    bubble->setVanish();
                }
                grGhostHouse* ground = static_cast<grGhostHouse*>(this->getGround(0));
                u32 nodeIndex = ground->getNodeIndex(0, "Bubbles");
                nw4r::g3d::ResNodeData* resNodeDataSW = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
                nw4r::g3d::ResNodeData* resNodeDataNE = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 2)).ptr();
                this->eventStartTimer = (resNodeDataNE->m_translation.m_x - resNodeDataSW->m_translation.m_x) / ghostHouseData->bubbleSpeedX;
            }
                break;
            case Event_Eerie:
                for (int i = 0; i < ghostHouseData->numEeries; i++) {
                    grGhostHouseEerie* eerie = static_cast<grGhostHouseEerie*>(this->getGround(this->eerieStartGroundIndex + i));
                    eerie->setVanish();
                }
                this->eventStartTimer = 120;
                break;
            default:
                this->eventStartTimer = 120;
                break;
        }
        this->nextEvent = event;
    }
}

// TODO: Make random event be based on frame timer so that can have some control over event (some frames can be completely random)

void Ground::setStageData(void* stageData) {
   this->m_stageData = stageData;
}
void stGhostHouse::startFighterEvent() {
   return;
}
int stGhostHouse::initializeFighterAttackRatio() {
   return 0;
}
int stGhostHouse::helperStarWarp(){
   return 0;
}
bool stGhostHouse::isSimpleBossBattleMode() {
   return false;
}
bool stGhostHouse::isBossBattleMode() {
   return false;
}
bool stGhostHouse::isCameraLocked() {
   return true;
}
void stGhostHouse::notifyTimmingGameStart(){
   return;
}
float stGhostHouse::getFrameRuleTime() {
   return this->m_frameRuleTime;
}
void stGhostHouse::setFrameRuleTime(float newTime) {
   this->m_frameRuleTime = newTime;
}
bool stGhostHouse::isNextStepBgmEqualNowStepBgmFromFlag() {
   return false;
}
float stGhostHouse::getBgmPlayOffsetFrame(){
   return BGM_PLAY_OFFSET_FRAME;
}
float stGhostHouse::getBgmVolume() {
   return BGM_VOLUME;
}
void stGhostHouse::setBgmChange(float unk, u32 unk1, u32 unk2) {
   this->m_unk2 = unk1;
   this->m_unk3 = unk2;
   this->m_unk4 = unk;
}
void stGhostHouse::getBgmChangeID(u32 unk1, float unk2) {
   unk1 = this->m_unk3;
   unk2 = this->m_unk4;
}
bool stGhostHouse::isBgmChange() {
   return this->m_unk2;
}
int stGhostHouse::getBgmOptionID() {
   return 0;
}
int stGhostHouse::getNowStepBgmID() {
   return 0;
}
int stGhostHouse::getBgmID() {
   return 0;
}
int stGhostHouse::getBgmIDOverload() {
   return 0;
}
void stGhostHouse::appearanceFighterLocal() {
   return;
}
// TODO
// stubbed because i can't figure out where this is called
int stGhostHouse::getScrollDir(u32 unk1) {
   return 0;
}
int stGhostHouse::getDefaultLightSetIndex(){
   return 0x14;
}
stRange* stGhostHouse::getAIRange() {
    return &this->m_aiRange;
}
bool stGhostHouse::isAdventureStage(){
   return false;
}
int stGhostHouse::getPokeTrainerDrawLayer() {
   return 0;
}
float stGhostHouse::getPokeTrainerPositionZ() {
   return POKETRAINER_Z;
}
void stGhostHouse::getPokeTrainerPointData(int * unk, int unk2){
   return;
}
int stGhostHouse::getPokeTrainerPointNum() {
   return 0;
}
bool stGhostHouse::isReStartSamePoint() {
   return true;
}
grGimmickWindData2nd* stGhostHouse::getWind2ndOnlyData() {
    return m_windAreaData2nd;
}
bool stGhostHouse::isBamperVector() {
   return true;
}
int stGhostHouse::getFinalTechniqColor() {
   return 0x14000496;
}

template<srStageKind I, typename T>
T* stClassInfoImpl<I, T>::create(){
    T* stage = new(Heaps::StageInstance) T(I);
    return stage;
}

template<srStageKind I, typename T>
stClassInfoImpl<I, T>::~stClassInfoImpl() {
   setClassInfo(I,0);
}

template<srStageKind I, typename T>
void stClassInfoImpl<I, T>::preload() {
   return;
}

// TODO: in timed or coin mode, get score for killing boos