#include "st_izumi.h"
#include "st_izumi_data.h"
#include "gr_izumi.h"
#include "gr_izumi_spout.h"
#include "gr_izumi_star.h"
#include "gr_izumi_star_item.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <OS/OSError.h>

static stClassInfoImpl<Stages::Final, stIzumi> classInfo = stClassInfoImpl<Stages::Final, stIzumi>();

stIzumi* stIzumi::create()
{
    stIzumi* stage = new (Heaps::StageInstance) stIzumi();
    return stage;
}
bool stIzumi::loading()
{
    return true;
}
void stIzumi::update(float deltaFrame)
{
    return;
}

void stIzumi::createObj()
{
    testStageParamInit(m_fileData, 0xA);
    testStageDataInit(m_fileData, 0x14, 1);
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->m_stageData);

    grIzumi* ground = grIzumi::create(0, "", "grIzumiMainBg");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setStageData(m_stageData);
        ground->startup(m_fileData, 0, 0);
        ground->setDontMoveGround();
    }
    grIzumiSpout* spout = grIzumiSpout::create(80, "", "grIzumiSpout");
    if (spout != NULL)
    {
        addGround(spout);
        spout->setStageData(m_stageData);
        spout->startup(m_fileData, 0, 0);
        spout->setDontMoveGround();
        spout->fountainInit(0);
        createCollision(m_fileData, 3, spout);
    }
    spout = grIzumiSpout::create(81, "", "grIzumiSpout");
    if (spout != NULL)
    {
        addGround(spout);
        spout->setStageData(m_stageData);
        spout->startup(m_fileData, 0, 0);
        spout->setDontMoveGround();
        spout->fountainInit(1);
        createCollision(m_fileData, 4, spout);

    }
    ground = grIzumi::create(3, "", "grIzumiMainBg");
    if (ground != NULL)
    {
        addGround(ground);
        ground->setStageData(m_stageData);
        ground->startup(m_fileData, 0, 0);
        ground->setDontMoveGround();
    }
    for(int i = 0; i < izumiData->numStars; i++) {
        grIzumiStar* star = grIzumiStar::create(4, "", "grIzumiStar");
        if (star != NULL)
        {
            addGround(star);
            star->setStageData(m_stageData);
            star->startup(m_fileData, 0, 0);
        }
    }
    for(int i = 0; i < izumiData->numItemStars; i++) {
        grIzumiStarItem* star = grIzumiStarItem::create(4, "", "grIzumiStarItem", this);
        if (star != NULL)
        {
            addGround(star);
            star->setStageData(m_stageData);
            star->startup(m_fileData, 0, 0);
        }
    }

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

    g_ecMgr->setDrawPrio(1);
    g_ecMgr->setEffect((EfID)0x330005);
    g_ecMgr->setDrawPrio(0xffffffff);
}

u8 stIzumi::getIteamDropStatus() {
    if (grIzumiStarItem::getRainMode() != grIzumiStarItem::Rain_None) {
        return false;
    }
    return true;
}


void stIzumi::startFighterEvent()
{
    return;
}
int stIzumi::initializeFighterAttackRatio()
{
    return 0;
}
int stIzumi::helperStarWarp()
{
    return 0;
}
bool stIzumi::isSimpleBossBattleMode()
{
    return false;
}
bool stIzumi::isBossBattleMode()
{
    return false;
}
bool stIzumi::isCameraLocked()
{
    return true;
}
void stIzumi::notifyTimmingGameStart()
{
    return;
}
float stIzumi::getFrameRuleTime()
{
    return this->m_frameRuleTime;
}
void stIzumi::setFrameRuleTime(float newTime)
{
    this->m_frameRuleTime = newTime;
}
bool stIzumi::isNextStepBgmEqualNowStepBgmFromFlag()
{
    return false;
}
float stIzumi::getBgmPlayOffsetFrame()
{
    return BGM_PLAY_OFFSET_FRAME;
}
float stIzumi::getBgmVolume()
{
    return BGM_VOLUME;
}
void stIzumi::setBgmChange(float unk, u32 unk1, u32 unk2)
{
    this->m_unk2 = unk1;
    this->m_unk3 = unk2;
    this->m_unk4 = unk;
}
void stIzumi::getBgmChangeID(u32 unk1, float unk2)
{
    unk1 = this->m_unk3;
    unk2 = this->m_unk4;
}
bool stIzumi::isBgmChange()
{
    return this->m_unk2;
}
int stIzumi::getBgmOptionID()
{
    return 0;
}
int stIzumi::getNowStepBgmID()
{
    return 0;
}
int stIzumi::getBgmID()
{
    return 0;
}
int stIzumi::getBgmIDOverload()
{
    return 0;
}
void stIzumi::appearanceFighterLocal()
{
    return;
}
// TODO
// stubbed because i can't figure out where this is called
int stIzumi::getScrollDir(u32 unk1)
{
    return 0;
}
int stIzumi::getDefaultLightSetIndex()
{
    return 0x14;
}
Rect2D* stIzumi::getAIRange()
{
    return &this->m_aiRange;
}
bool stIzumi::isAdventureStage()
{
    return false;
}
int stIzumi::getPokeTrainerDrawLayer()
{
    return 0;
}
float stIzumi::getPokeTrainerPositionZ()
{
    return POKETRAINER_Z;
}
void stIzumi::getPokeTrainerPointData(int* unk, int unk2)
{
    return;
}
int stIzumi::getPokeTrainerPointNum()
{
    return 0;
}
bool stIzumi::isReStartSamePoint()
{
    return true;
}
grGimmickWindData2nd* stIzumi::getWind2ndOnlyData()
{
    return m_windAreaData2nd;
}
bool stIzumi::isBamperVector()
{
    return true;
}
int stIzumi::getFinalTechniqColor()
{
    return 0x14000496;
}

ST_CLASS_INFO;