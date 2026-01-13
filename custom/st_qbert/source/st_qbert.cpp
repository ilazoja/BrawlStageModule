#include <memory.h>
#include <st/st_class_info.h>
#include "st_qbert.h"
#include <gf/gf_camera.h>
#include <snd/snd_system.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>
#include <hkmath/hkMath.h>

static stClassInfoImpl<Stages::Final, stQbert> classInfo = stClassInfoImpl<Stages::Final, stQbert>();

stQbert* stQbert::create() {
    stQbert* stage = new(Heaps::StageInstance) stQbert(Stages::Final);
    return stage;
}
bool stQbert::loading(){
    return true;
}

void stQbert::notifyEventInfoGo() {
    grQbertEnemy* enemy;
    for (u8 i = 44; i < NUM_ENEMIES + 44; i++) {
        enemy = (grQbertEnemy*)this->getGround(i);
        enemy->setStart();
    }
}

void stQbert::createObj() {

    this->gameRule = g_ftManager->m_gameRule;
    if (this->gameRule == Game_Rule_Coin) {
        g_ftManager->m_gameRule = Game_Rule_Time;
    }

    testStageParamInit(m_fileData, 210);
    testStageDataInit(m_fileData, 220, 1);

    this->createObjBg(0);
    this->createObjHud(1);
    for (int cubeIndex = 2; cubeIndex < NUM_BLOCKS + 2; cubeIndex++) {
        this->createObjCube(cubeIndex, cubeIndex);
    }
    for (int diskIndex = 30; diskIndex < NUM_DISKS + 30; diskIndex++) {
        this->createObjDisk(diskIndex, diskIndex, diskIndex - 30);
    }

    grQbertAlien* alien = this->createObjAlien(44);
    this->createObjCoily(45, alien);
    this->createObjGreen(46);
    this->createObjRed(47);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        for (int j = 0; j < NUM_SCORE_DIGITS; j++) {
            this->createObjScore(48, i, j);
        }
    }
    this->createObjRound(48, 0);
    this->createObjRound(48, 1);

    initCameraParam();
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
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 230);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);

    stQbertStageData* qbertStageData = (stQbertStageData*)this->m_stageData;
    this->maxDisksActive = randi(maxDisksActive) + 1;
    this->diskTimer = randf()*(qbertStageData->diskMaxRespawnFrames - qbertStageData->diskMinRespawnFrames) + qbertStageData->diskMinRespawnFrames;

    // setup orthogonal camera
    gfCameraManager* cameraManager = gfCameraManager::getManager();
    gfCamera* camera = &cameraManager->m_cameras[0];
    camera->m_projectionType = gfCamera::Projection_Orthogonal;
    camera->m_orthoParam.m_top = qbertStageData->orthogonalCameraZoom/2;
    camera->m_orthoParam.m_bottom = -qbertStageData->orthogonalCameraZoom/2;
    camera->m_orthoParam.m_left = -(4.0/3.0)*qbertStageData->orthogonalCameraZoom/2.0;
    camera->m_orthoParam.m_right = (4.0/3.0)*qbertStageData->orthogonalCameraZoom/2.0;

    this->soundGenerator.playSE(snd_se_stage_Madein_01, 0x0, 0x0, 0xffffffff);
}

void stQbert::createObjBg(int mdlIndex) {
    grQbertBackground* ground = grQbertBackground::create(mdlIndex, "", "grQbertMainBg");
    if(ground != NULL){
        addGround(ground);
        ground->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

void stQbert::createObjHud(int mdlIndex) {
    grQbertHud* hud = grQbertHud::create(mdlIndex, "", "grQbertHud");
    if(hud != NULL){
        addGround(hud);
        hud->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        hud->setStageData(m_stageData);
        hud->initializeEntity();
        hud->startEntityAutoLoop();
        hud->setScorePosWork(this->scorePositions);
    }
}

void stQbert::createObjCube(int mdlIndex, int collIndex) {
    grQbertCube* cube = grQbertCube::create(mdlIndex, "", "grQbertCube");
    if(cube != NULL){
        addGround(cube);
        cube->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        cube->setStageData(m_stageData);
        cube->setDontMoveGround();
        cube->initializeEntity();
        cube->startEntity();
        cube->setNumBlocksPerTeamWork(this->numBlocksPerTeam);
        cube->setLastLanderEntryIdForTeamWork(this->lastLanderEntryIdForTeam);
        cube->setTeamScoresWork(this->teamScores, this->gameRule);
        createCollision(m_fileData, collIndex, cube);
    }
}

void stQbert::createObjDisk(int mdlIndex, int collIndex, int diskIndex) {
    grQbertDisk* disk = grQbertDisk::create(mdlIndex, "", "grQbertDisk");
    if(disk != NULL){
        addGround(disk);
        disk->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        disk->setStageData(m_stageData);
        disk->initializeEntity();
        disk->startEntity();
        disk->setIsActiveWork(&this->disksActive[diskIndex]);
        disk->setMotion(0);
        createCollision(m_fileData, collIndex, disk);
        disk->setEnableCollisionStatus(false);
    }
}

grQbertAlien* stQbert::createObjAlien(int mdlIndex) {
    grQbertAlien* alien = grQbertAlien::create(mdlIndex, "", "grQbertAlien", this);
    if(alien != NULL){
        addGround(alien);
        alien->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        alien->setStageData(m_stageData);
        alien->initializeEntity();
        alien->startEntity();
        alien->setStartPos();
        alien->setTeamScoresWork(this->teamScores, this->gameRule);
    }
    return alien;
}

void stQbert::createObjCoily(int mdlIndex, grQbertAlien* enemyTarget) {
    grQbertCoily* coily = grQbertCoily::create(mdlIndex, "", "grQbertCoily", this, enemyTarget);
    if(coily != NULL){
        addGround(coily);
        coily->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        coily->setStageData(m_stageData);
        coily->initializeEntity();
        coily->startEntity();
        coily->setStartPos();
        coily->setTeamScoresWork(this->teamScores, this->gameRule);
    }
}

void stQbert::createObjGreen(int mdlIndex) {
    grQbertGreen* green = grQbertGreen::create(mdlIndex, "", "grQbertGreen", this);
    if(green != NULL){
        addGround(green);
        green->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        green->setStageData(m_stageData);
        green->initializeEntity();
        green->startEntity();
        green->setStartPos();
        green->setImmobilizeStateWork(&this->immobilizeState);
        green->setTeamScoresWork(this->teamScores, this->gameRule);
    }
}

void stQbert::createObjRed(int mdlIndex) {
    grQbertRed* red = grQbertRed::create(mdlIndex, "", "grQbertRed", this);
    if(red != NULL){
        addGround(red);
        red->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        red->setStageData(m_stageData);
        red->initializeEntity();
        red->startEntity();
        red->setStartPos();
        red->setTeamScoresWork(this->teamScores, this->gameRule);
    }
}

void stQbert::createObjScore(int mdlIndex, int player, int type) {
    grQbertScore* score = grQbertScore::create(mdlIndex, "StgDonkey_Suuji", "grQbertScore");
    if(score != NULL){
        addGround(score);
        score->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        score->setStageData(m_stageData);
        score->setType(type);
        score->setPosWork(&this->scorePositions[player*NUM_SCORE_DIGITS + type]);
        score->setScoreWork(&this->teamScores[player]);
    }
}

void stQbert::createObjRound(int mdlIndex, int type) {
    grQbertScore* score = grQbertScore::create(mdlIndex, "StgDonkey_Suuji", "grQbertRound");
    if(score != NULL){
        addGround(score);
        score->startup(m_fileData,0,gfSceneRoot::Layer_Ground);
        score->setStageData(m_stageData);
        score->setType(type);
        score->setPosWork(&this->scorePositions[4*NUM_SCORE_DIGITS + type]);
        score->setScoreWork(&this->round);
    }
}

void stQbert::update(float frameDelta){
    this->updateCubes(frameDelta);
    this->updateDisks(frameDelta);
    this->updateEnemies(frameDelta);
    this->updateBgm(frameDelta);
}

void stQbert::updateCubes(float frameDelta) {

    // Check if time to give score
    if (this->winTimer > 0) {
        this->winTimer -= frameDelta;
        if (this->winTimer <= 0) {
            if (this->winningTeamId > 0 && this->winningTeamId - 1 < NUM_PLAYERS) {
                u32 pointsWon = STARTING_COMPLETION_POINTS
                                + ADDED_POINTS_PER_ROUND*this->round
                                + REMAINING_DISKS_POINTS*this->numDisksActive;
                this->teamScores[this->winningTeamId - 1] += pointsWon;
                if (this->gameRule == Game_Rule_Coin && this->lastLanderEntryIdForTeam[this->winningTeamId] > -1) {
                    g_ftManager->pickupCoin(this->lastLanderEntryIdForTeam[this->winningTeamId], pointsWon);
                }
            }
        }
    }

    // Check if all blocks have been coloured by a team
    stQbertStageData* qbertStageData = (stQbertStageData*)this->m_stageData;
    for (u8 team = 1; team < NUM_TEAMS; team++) {
        if (this->numBlocksPerTeam[team] >= NUM_BLOCKS) {
            g_sndSystem->setBGMVol(true, 0);
            this->soundGenerator.playSE(snd_se_stage_Madein_bad_04, 0x0, 0x0, 0xffffffff);
            for (u8 blockNum = 0; blockNum < NUM_BLOCKS; blockNum++) {
                grQbertCube* cube = (grQbertCube*)this->getGround(blockNum + 2);
                cube->setWin();
            }
            for (u8 i = 0; i < g_ftManager->getEntryCount(); i++) {
                // set reward for team
                int entryId = g_ftManager->getEntryIdFromIndex(i);
                int teamNumber = g_ftManager->getTeam(entryId, false, false) + 1;
                if (this->gameRule != Game_Rule_Coin && (teamNumber == team || team > 5)) {
                    g_ftManager->setCurry(entryId);
                }
            }
            this->winTimer = qbertStageData->winFrames;
            this->bgmTimer = hkMath::max2(this->bgmTimer, qbertStageData->immobilizeFrames);
            this->winningTeamId = team;
            this->round++;
            if (this->round > MAX_ROUNDS) {
                this->round = MAX_ROUNDS;
            }
            this->maxDisksActive = randi(maxDisksActive) + 1;
            grQbertAlien* alien = (grQbertAlien*)this->getGround(44);
            alien->setTeam(STARTING_TEAM_ID);
            break;
        }
    }
}

void stQbert::updateDisks(float frameDelta) {

    // Check if disks should be activated
    u8 inactiveDiskIndices[NUM_DISKS];
    u8 numInactiveDisks = 0;
    for (u8 diskIndex = 0; diskIndex < NUM_DISKS; diskIndex++) {
        if (!this->disksActive[diskIndex]) {
            inactiveDiskIndices[numInactiveDisks] = diskIndex;
            numInactiveDisks++;
        }
    }
    this->numDisksActive = NUM_DISKS - numInactiveDisks;
    stQbertStageData* qbertStageData = (stQbertStageData*)this->m_stageData;
    if (numInactiveDisks > 0 && this->numDisksActive < this->maxDisksActive) {
        this->diskTimer -= frameDelta;
        if (this->diskTimer <= 0) {
            int diskIndex = inactiveDiskIndices[randi(numInactiveDisks)];
            this->disksActive[diskIndex] = true;
            this->diskTimer = randf()*(qbertStageData->diskMaxRespawnFrames - qbertStageData->diskMinRespawnFrames) + qbertStageData->diskMinRespawnFrames;
        }
    }
}

void stQbert::updateEnemies(float frameDelta) {
    // Check if green orb was collected
    stQbertStageData* qbertStageData = (stQbertStageData*)this->m_stageData;
    if (this->immobilizeState > 0) {
        g_sndSystem->setBGMVol(true, 0);
        this->soundGenerator.playSE(snd_se_stage_Madein_good_06, 0x0, 0x0, 0xffffffff);
        grQbertBackground* background = (grQbertBackground*)this->getGround(0);
        background->setImmobilize(qbertStageData->immobilizeFrames);
        for (u8 i = 43 + this->immobilizeState; i < NUM_ENEMIES + 44; i++) {
            grQbertEnemy* enemy = (grQbertEnemy*)this->getGround(i);
            enemy->setImmobilize(qbertStageData->immobilizeFrames);
        }
        this->immobilizeState = Immobilize_None;
        this->bgmTimer = hkMath::max2(this->bgmTimer, qbertStageData->immobilizeFrames);
    }
}

void stQbert::updateBgm(float frameDelta) {
    // Check if time to restore music to full volume
    if (this->bgmTimer > 0) {
        this->bgmTimer -= frameDelta;
        if (this->bgmTimer <= 0) {
            g_sndSystem->setBGMVol(true, this->m_stageParam->m_bgmVolume);
        }
    }
}

bool stQbert::isBamperVector() {
   return true;
}
GXColor stQbert::getFinalTechniqColor() {
   return (GXColor){0x14000496};
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