#include "st_boss.h"
#include <OS/OSError.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <gf/gf_module.h>
#include <em/em_manager.h>
#include <em/em_weapon_manager.h>
#include <gf/gf_heap_manager.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>

// TODO: Patch enemy pop so that can hitbox can affect any team (but can't hit fighters) (or even better change the team based on who killed the enemy)
// TODO: Destroy enemy if they go past blast zone
// TODO: Test different enemies

static stClassInfoImpl<Stages::Final, stBoss> classInfo = stClassInfoImpl<Stages::Final, stBoss>();

stBoss* stBoss::create()
{
    stBoss* stage = new (Heaps::StageInstance) stBoss();
    return stage;
}
bool stBoss::loading()
{
    return true;
}

void stBoss::notifyEventInfoGo() {
    emManager::create(0x1e,0x14,0);
    //gfHeapManager::dumpList();
    emWeaponManager::create();
    emWeaponManager* weaponManager = emWeaponManager::getInstance();
    weaponManager->clean();
    weaponManager->m_list1.clear();
    weaponManager->m_list2.clear();
    weaponManager->m_numStageObjects = NUM_WEAPON_STAGE_OBJECTS
    weaponManager->m_stageObjects = new (Heaps::StageInstance) wnemSimple[weaponManager->m_numStageObjects];
    for (int i = 0; i < weaponManager->m_numStageObjects; i++) {
        weaponManager->m_list1.addTail(&weaponManager->m_stageObjects[i]);
    }
    weaponManager->m_32 = false;


    stBossData* bossData = static_cast<stBossData*>(this->m_stageData);
    grBoss* ground = static_cast<grBoss*>(this->getGround(0));

    for (int i = 0; i < NUM_ENEMY_TYPES; i++) {
        if (bossData->maxNumEnemies[i] > 0) {
            gfArchive* brres;
            gfArchive* param;
            gfArchive* enmCommon;
            gfArchive* primFaceBrres;
            this->getEnemyPac(&brres, &param, &enmCommon, &primFaceBrres, (EnemyKind)i);
            emManager* enemyManager = emManager::getInstance();
            int result = enemyManager->preloadArchive(param, brres, enmCommon, primFaceBrres, (EnemyKind)i, true);
            OSReport("Enemy %d archive preloaded result: %d \n", i, result);

            u32 nodeIndex;
            ground->getNodeIndexWithFormat(&nodeIndex, 0, "Enemy%d", i);
            nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(nodeIndex + 1)).ptr();
            this->enemySpawnTimers[i] = randi(resNodeData->m_scale.m_y - resNodeData->m_scale.m_x) + resNodeData->m_scale.m_x;
        }
    }
    this->isGo = true;
};

void stBoss::update(float deltaFrame)
{
    if (this->isGo) {
        stBossData* bossData = static_cast<stBossData*>(this->m_stageData);
        grBoss* ground = static_cast<grBoss*>(this->getGround(0));
        emManager* enemyManager = emManager::getInstance();

        for (int i = 0; i < NUM_ENEMY_TYPES; i++) {
            if (bossData->maxNumEnemies[i] > 0) {
                if (enemyManager->getEnemyCountFromKind((EnemyKind)i) < bossData->maxNumEnemies[i]) {

                    this->enemySpawnTimers[i] -= deltaFrame;
                    if (this->enemySpawnTimers[i] <= 0) {
                        this->enemySpawnTimers[i] = 0;

                        u32 startNodeIndex;
                        ground->getNodeIndexWithFormat(&startNodeIndex, 0, "Enemy%d", i);
                        u32 endNodeIndex;
                        ground->getNodeIndexWithFormat(&endNodeIndex, 0, "Enemy%d", i);
                        u32 chosenNodeIndex = randi(endNodeIndex - startNodeIndex + 1) + startNodeIndex + 1;

                        nw4r::g3d::ResNodeData* resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(chosenNodeIndex)).ptr();

                        emCreate create;
                        create.m_teamNo = 10000;
                        create.m_difficulty = resNodeData->m_rotation.m_y;
                        create.m_enemyKind = (EnemyKind)i;
                        create.m_startStatusKind = resNodeData->m_rotation.m_x;

                        Vec3f pos;
                        ground->getNodePosition(&pos, 0, chosenNodeIndex);
                        create.m_startPos = Vec3f(pos.m_x, pos.m_y, 0.0);

                        create.m_startLr = resNodeData->m_translation.m_z;
                        create.m_level = 1;
                        create.m_36 = 0.0;
                        create.m_territoryRange = (Rect2D){0.0, 0.0, 0.0, 0.0};
                        create.m_connectedTriggerId = 0;
                        create.m_epbm = NULL;
                        create.m_motionPath = NULL;
                        create.m_epsp = NULL;
                        create.m_parentCreateId = 0xFFFF;
                        //OSReport("Preload archive count result: %d \n", enemyManager->getPreloadArchiveCountFromKind(Enemy_Kuribo));

                        int result = enemyManager->createEnemy(&create);
                        OSReport("Enemy Create result: %d \n", result);

                        resNodeData = ground->m_sceneModels[0]->m_resMdl.GetResNode(int(startNodeIndex + 2)).ptr();
                        this->enemySpawnTimers[i] = randi(resNodeData->m_scale.m_y - resNodeData->m_scale.m_x) + resNodeData->m_scale.m_x;

                        if (bossData->bgmID != -1 && bossData->bgmID != this->regularBGMId && this->regularBGMId == g_sndSystem->getBGMId()) {
                            g_sndSystem->playBGM(bossData->bgmID, 0, true);
                        }

                    }
                }
            }
        }

        if (this->regularBGMId != g_sndSystem->getBGMId() && enemyManager->getEnemyCount() == 0) {
            g_sndSystem->playBGM(this->regularBGMId, 0, true);
        }
    }
    return;
}

void stBoss::createObj()
{

    testStageParamInit(m_fileData, 0xA);
    testStageDataInit(m_fileData, 0x14, 1);

    gfModuleManager* moduleManager = gfModuleManager::getInstance();
    int size;
    gfModuleHeader* moduleHeader = static_cast<gfModuleHeader*>(m_secondaryFileData->getData(Data_Type_Misc, 301, &size, 0xfffe));
    moduleManager->loadModuleRequestOnImage("sora_enemy.rel", Heaps::OverlayStage, moduleHeader, &size);

    this->primFaceIndex = randi(NUM_PRIM_FACES);
    this->regularBGMId = g_sndSystem->getBGMId();

    this->createObjGround(1);
    this->createObjGround(2);

    createCollision(m_fileData, 2, NULL);
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
    loadStageAttrParam(m_fileData, 0x1E);
    nw4r::g3d::ResFileData* scnData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xfffe));
    registScnAnim(scnData, 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", this->m_pokeTrainerPos, 0x0);
}

void stBoss::createObjGround(int mdlIndex) {
    grBoss* ground = grBoss::create(mdlIndex, "", "grBoss");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}


void stBoss::getEnemyPac(gfArchive **brres, gfArchive **param, gfArchive **enmCommon, gfArchive **primFaceBrres, EnemyKind enemyID) {
    int fileIndex = enemyID * 2;
    int nodeSize;
    *brres = NULL;
    *param = NULL;
    *enmCommon = NULL;
    *primFaceBrres = NULL;

    void* brresData = this->m_secondaryFileData->getData(Data_Type_Misc, fileIndex + 1, &nodeSize, (u32)0xfffe);
    *brres = new (Heaps::StageInstance) gfArchive();
    (*brres)->setFileImage(brresData, nodeSize, Heaps::StageResource);
    this->enemyArchives[enemyID*2 + 0] = *brres;

    void* paramData = this->m_secondaryFileData->getData(Data_Type_Misc, fileIndex, &nodeSize, (u32)0xfffe);
    *param = new (Heaps::StageInstance) gfArchive();
    (*param)->setFileImage(paramData, nodeSize, Heaps::StageResource);
    this->enemyArchives[enemyID*2 + 1] = *param;

    if (this->enemyCommonArchive == NULL) {
        void* enmCommonData = this->m_secondaryFileData->getData(Data_Type_Misc, 300, &nodeSize, (u32)0xfffe);
        *enmCommon = new (Heaps::StageInstance) gfArchive();
        (*enmCommon)->setFileImage(enmCommonData, nodeSize, Heaps::StageResource);
        this->enemyCommonArchive = *enmCommon;
    }
    else {
        *enmCommon = this->enemyCommonArchive;
    }

    if (enemyID == Enemy_Prim || enemyID == Enemy_Prim_Metal || enemyID == Enemy_Prim_Big || enemyID == Enemy_Prim_Boomerang || enemyID == Enemy_Prim_SuperScope || enemyID == Enemy_Prim_Sword) {
        if (this->primFaceArchive == NULL) {
            void* primFaceData = this->m_secondaryFileData->getData(Data_Type_Misc, 200 + primFaceIndex, &nodeSize, (u32)0xfffe);
            if (primFaceData == NULL) {
                primFaceData = this->m_secondaryFileData->getData(Data_Type_Misc, 200, &nodeSize, (u32)0xfffe);
            }
            *primFaceBrres = new (Heaps::StageInstance) gfArchive();
            (*primFaceBrres)->setFileImage(primFaceData, nodeSize, Heaps::StageResource);
            this->primFaceArchive = *primFaceBrres;
        }
        else {
            *primFaceBrres = this->primFaceArchive;
        }
    }

}

void stBoss::clearHeap() {
    emWeaponManager* weaponManager = emWeaponManager::getInstance();
    if (weaponManager != NULL) {
        weaponManager->reset();
        weaponManager->m_32 = true;
        emWeaponManager::remove();
    }
    emManager* enemyManager = emManager::getInstance();
    if (enemyManager != NULL) {
        emManager* enemyManager = emManager::getInstance();
        enemyManager->removeEnemyAll();
        enemyManager->removeArchiveAll();
        emManager::remove();
    }
    for (int i = 0; i < NUM_ENEMY_TYPES*2; i++) {
        if (this->enemyArchives[i] != NULL) {
            delete this->enemyArchives[i];
            this->enemyArchives[i] = NULL;
        }
    }
    if (this->enemyCommonArchive != NULL) {
        delete this->enemyCommonArchive;
        this->enemyCommonArchive = NULL;
    }
    if (this->primFaceArchive != NULL) {
        delete this->primFaceArchive;
        this->primFaceArchive = NULL;
    }

    gfModuleManager::getInstance()->destroy("sora_enemy.rel");
}

bool stBoss::isBamperVector()
{
    return true;
}
GXColor stBoss::getFinalTechniqColor()
{
    return (GXColor){0x14000496};
}


ST_CLASS_INFO