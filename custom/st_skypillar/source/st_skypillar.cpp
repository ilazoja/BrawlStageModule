#include "st_skypillar.h"
#include <OS/OSError.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <gf/gf_module.h>
#include <em/em_manager.h>
#include <em/em_weapon_manager.h>
#include <gf/gf_heap_manager.h>

// TODO: Patch enemy pop so that can hitbox can affect any team (but can't hit fighters) (or even better change the team based on who killed the enemy)
// TODO: Destroy enemy if they go past blast zone
// TODO: Test different enemies

static stClassInfoImpl<Stages::Final, stSkyPillar> classInfo = stClassInfoImpl<Stages::Final, stSkyPillar>();

stSkyPillar* stSkyPillar::create()
{
    stSkyPillar* stage = new (Heaps::StageInstance) stSkyPillar();
    return stage;
}
bool stSkyPillar::loading()
{
    return true;
}

void stSkyPillar::notifyEventInfoGo() {
    emManager::create(0x1e,0x14,0);
    //gfHeapManager::dumpList();
    emWeaponManager::create();
    emWeaponManager* weaponManager = emWeaponManager::getInstance();
    weaponManager->clean();
    weaponManager->m_list1.clear();
    weaponManager->m_list2.clear();
    weaponManager->m_numStageObjects = 0xf; //0x1e;
    weaponManager->m_stageObjects = new (Heaps::StageInstance) wnemSimple[weaponManager->m_numStageObjects];
    for (int i = 0; i < weaponManager->m_numStageObjects; i++) {
        weaponManager->m_list1.addTail(&weaponManager->m_stageObjects[i]);
    }
    weaponManager->m_32 = false;


    gfArchive* brres;
    gfArchive* param;
    gfArchive* enmCommon;
    gfArchive* primFaceBrres;
    this->getEnemyPac(&brres, &param, &enmCommon, &primFaceBrres, Enemy_Boss_Rayquaza); // Enemy_Kuribo
    emManager* enemyManager = emManager::getInstance();
    int result = enemyManager->preloadArchive(param, brres, enmCommon, primFaceBrres, Enemy_Boss_Rayquaza, true); // Enemy_Kuribo
    OSReport("Enemy archive preloaded result: %d \n", result);
    this->isGo = true;
};

void stSkyPillar::update(float frameDiff)
{
    if (this->isGo && !this->testCreated) {
        if (this->timer > 300.0) {
            this->testCreated = true;
            emManager* enemyManager = emManager::getInstance();
            emCreate create;
            create.m_teamNo = 10000;
            create.m_difficulty = 15;
            create.m_enemyKind = Enemy_Kuribo;
            create.m_startStatusKind = 2;
            create.m_startPos = Vec3f(70.0, 0.01, 0.0);
            create.m_startLr = -1.0; //1.0;
            create.m_level = 1;
            create.m_36 = 0.0;
            create.m_territoryRange = (Rect2D){0.0, 0.0, 0.0, 0.0};
            create.m_connectedTriggerId = 0;
            create.m_epbm = NULL;
            create.m_motionPath = NULL;
            create.m_epsp = NULL;
            create.m_parentCreateId = 0xFFFF;
            //OSReport("Preload archive count result: %d \n", enemyManager->getPreloadArchiveCountFromKind(Enemy_Kuribo));
            //int result = enemyManager->createEnemy(&create);

            create.m_startStatusKind = 2;
            create.m_enemyKind = Enemy_Boss_Rayquaza;
            int result = enemyManager->createEnemy(&create);
            OSReport("Enemy Create result: %d \n", result);
        }
        else {
            this->timer += frameDiff;
        }
    }
    return;
}

void stSkyPillar::createObj()
{
    gfModuleManager* moduleManager = gfModuleManager::getInstance();
    int size;
    gfModuleHeader* moduleHeader = static_cast<gfModuleHeader*>(m_fileData->getData(Data_Type_Misc, 41, &size, 0xfffe));
    moduleManager->loadModuleRequestOnImage("sora_enemy.rel", Heaps::OverlayStage, moduleHeader, &size);
    //moduleManager->setTempolaryLoadHeap(Heaps::FighterTechqniq);
    //gfModuleManager::LoadRequestResult requestResult;
    //gfModuleManager::loadModuleRequest(&requestResult, moduleManager, "sora_enemy_rayquaza.rel", Heaps::OverlayStage, 1, 0);

    testStageParamInit(m_fileData, 0xA);
    testStageDataInit(m_fileData, 0x14, 1);

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

void stSkyPillar::createObjGround(int mdlIndex) {
    grSkyPillar* ground = grSkyPillar::create(mdlIndex, "", "grSkyPillar");
    if (ground != NULL)
    {
        addGround(ground);
        ground->startup(m_fileData, 0, 0);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}


void stSkyPillar::getEnemyPac(gfArchive **brres, gfArchive **param, gfArchive **enmCommon, gfArchive **primFaceBrres, EnemyKind enemyID) {
    int fileIndex = enemyID * 2;
    int nodeSize;
    *brres = NULL;
    *param = NULL;
    *enmCommon = NULL;
    *primFaceBrres = NULL;

    void* brresData = this->m_secondaryFileData->getData(Data_Type_Misc, fileIndex + 1, &nodeSize, (u32)0xfffe);
    *brres = new (Heaps::StageInstance) gfArchive();
    (*brres)->setFileImage(brresData, nodeSize, Heaps::StageResource);
    this->enemyArchives[0] = *brres;

    void* paramData = this->m_secondaryFileData->getData(Data_Type_Misc, fileIndex, &nodeSize, (u32)0xfffe);
    *param = new (Heaps::StageInstance) gfArchive();
    (*param)->setFileImage(paramData, nodeSize, Heaps::StageResource);
    this->enemyArchives[1] = *param;

    void* enmCommonData = this->m_secondaryFileData->getData(Data_Type_Misc, 300, &nodeSize, (u32)0xfffe);
    *enmCommon = new (Heaps::StageInstance) gfArchive();
    (*enmCommon)->setFileImage(enmCommonData, nodeSize, Heaps::StageResource);
    this->enemyCommonArchive = *enmCommon;
}

void stSkyPillar::clearHeap() {
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

    gfModuleManager::getInstance()->destroy("sora_enemy_vs.rel");
}

bool stSkyPillar::isBamperVector()
{
    return true;
}
GXColor stSkyPillar::getFinalTechniqColor()
{
    return (GXColor){0x14000496};
}


ST_CLASS_INFO