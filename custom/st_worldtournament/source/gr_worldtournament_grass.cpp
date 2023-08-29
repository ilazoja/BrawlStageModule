#include "gr_worldtournament_grass.h"
#include <OS/OSError.h>
#include <ec/ec_mgr.h>
#include <memory.h>
#include <so/so_value_accesser.h>
#include <ft/ft_audience_manager.h>
#include <ai/ai_mgr.h>

grWorldTournamentGrass* grWorldTournamentGrass::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grWorldTournamentGrass* ground = new (Heaps::StageInstance) grWorldTournamentGrass(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grWorldTournamentGrass::startup(gfArchive* data, u32 unk1, u32 unk2) {
    grYakumono::startup(data, unk1, unk2);
    this->areaData = (soAreaData){ 0, 0x15, 0, 0, 0, 0, (Vec2f){0.0, -10.0}, (Vec2f){1000.0,5.0}};
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(GimmickKind_AreaCommon,-1);
    trigger->setObserveYakumono(this->m_yakumono);
}

void grWorldTournamentGrass::update(float deltaFrame)
{
    if (this->collTimer >= 0) {
        this->collTimer -= deltaFrame;
        if (this->collTimer <= 0) {
            this->setEnableCollisionStatus(false);
        }
    }
}

void grWorldTournamentGrass::onGimmickEvent(soGimmickEventInfo* eventInfo, int* taskId)
{
    this->collTimer = 2.0;
    this->setEnableCollisionStatus(true); // Enable collision only when close to grass so that CPU does not want to go on it
}

void grWorldTournamentGrass::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    int unk = 1;
    if (this->isCollisionStatusOwnerTask(collStatus, &unk)) {
        Fighter* fighter = (Fighter*)gfTask::getTask(collStatus->m_taskId);
        if (fighter != NULL) {
            if (!fighter->m_moduleAccesser->getWorkManageModule()->isFlag(0x12000018)) {
                //fighter->toKnockOut();
                this->toKnockOut(fighter);
            }
            else if (fighter->m_moduleAccesser->getStatusModule()->getStatusKind() == ftStatus::Lying) {
                soMotionModule* motionModule = fighter->m_moduleAccesser->getMotionModule();
                if (motionModule->getEndFrame() - motionModule->getFrame() <= 1) {
                    soPostureModule* postureModule = fighter->m_moduleAccesser->getPostureModule();
                    Vec3f pos = postureModule->getPos();
                    float scale = postureModule->getScale(); // TODO: Get model scale?
                    g_ecMgr->setEffect(0x4c, &pos, NULL, &(Vec3f){scale, scale, scale});
                    fighter->toDead(-1);
                }
            }
        }
    }
}

void grWorldTournamentGrass::toKnockOut(Fighter* fighter) {
    soModuleAccesser* moduleAccesser = fighter->m_moduleAccesser;
    soDamageAttackerInfo attackerInfo;
    if (!fighter->getOwner()->isSubOwner()) {
        if (g_ftManager->isProcessHeartSwap(fighter->m_entryId)) {
            moduleAccesser->getDamageModule()->getAttackerInfo(&attackerInfo);
            g_ftManager->toKnockOutHeartSwapOpposite(fighter->m_entryId, &attackerInfo);
            return;
        }
        //moduleAccesser->getEffectModule()->reqCommon(0.0, 0x28);
        soSoundModule::SoundIdData* soundData = moduleAccesser->getSoundModule()->getSoundIdData();
        moduleAccesser->getSoundModule()->playSE(soundData->soundIdLists[3].sndIDs[2], 1, 1, 0);

        //moduleAccesser->getEffectModule()->reqScreen(7);
        //int wholeFrame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a4f, 0);
        //int ivar6 = soValueAccesser::getConstantInt(moduleAccesser, 0x5a4e, 0);
        //moduleAccesser->getSlowModule()->setWhole(ivar6, wholeFrame);
        moduleAccesser->getDamageModule()->getAttackerInfo(&attackerInfo);
        if (attackerInfo.m_indirectAttackerSoKind == StageObject_Fighter && g_ftManager->isValidEntryId(attackerInfo.m_indirectAttackerEntryId)) {
            g_ftAudienceManager->m_audience->checkCheerDefeat(attackerInfo.m_indirectAttackerEntryId);
            g_ftManager->setBeat(fighter->m_entryId, attackerInfo.m_indirectAttackerEntryId);
        }
        else {
            g_ftManager->setSuicide(fighter->m_entryId);
        }
        g_ftManager->setDead(fighter->m_entryId, 5, -1);
        fighter->m_outsideEventPresenter.notifyOutsideEventKnockout();
    }
    fighter->setCurry(false, -1);
    moduleAccesser->getCollisionHitModule()->setCheckCatch(0,0);
    moduleAccesser->getWorkManageModule()->onFlag(0x12000018);
    moduleAccesser->getControllerModule()->setOff(true);
    ftStatus::Kind statusKind = ftStatus::Tumble;
    if (moduleAccesser->getSituationModule()->getKind() == 0) {
        statusKind = ftStatus::Hit_Wall_Tumble;
    }
    moduleAccesser->getStatusModule()->changeStatusRequest(statusKind, moduleAccesser);
}
