#include "gr_worldtournament_concrete.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>

grWorldTournamentConcrete* grWorldTournamentConcrete::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grWorldTournamentConcrete* ground = new (Heaps::StageInstance) grWorldTournamentConcrete(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grWorldTournamentConcrete::startup(gfArchive* data, u32 unk1, u32 unk2) {
    grYakumono::startup(data, unk1, unk2);
    this->areaData = (soAreaData){ 0, gfArea::Stage_Group_Gimmick_Normal, 0, 0, 0, 0, Vec2f(0.0, -115.0), Vec2f(1000.0,100.0)};
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Common,-1);
    trigger->setObserveYakumono(this->m_yakumono);
}

void grWorldTournamentConcrete::update(float deltaFrame)
{

}

void grWorldTournamentConcrete::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    Fighter* fighter = static_cast<Fighter*>(gfTask::getTask(*taskId));
    if (fighter != NULL) {
        Vec3f pos = soExternalValueAccesser::getPos(fighter);
        pos.m_y = -10.0;
        fighter->warp(&pos, 1.0, false);
    }
}


void grWorldTournamentConcrete::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    CategoryFlag categoryFlag(GROUND_COLL_STATUS_OWNER_TASK_CATEGORY_MASK_FIGHTER);
    if (this->isCollisionStatusOwnerTask(collStatus, &categoryFlag)) {
        Fighter* fighter = (Fighter*)gfTask::getTask(collStatus->m_taskId);
        if (fighter != NULL) {
            if (fighter->m_moduleAccesser->getWorkManageModule().isFlag(0x12000018)) {
                soMotionModule& motionModule = fighter->m_moduleAccesser->getMotionModule();
                if (motionModule.getEndFrame() - motionModule.getFrame() <= 1) {
                    soPostureModule& postureModule = fighter->m_moduleAccesser->getPostureModule();
                    Vec3f pos = postureModule.getPos();
                    float scale = postureModule.getScale(); // TODO: Get model scale?
                    Vec3f effectScale = Vec3f(scale, scale, scale);
                    g_ecMgr->setEffect(ef_ptc_common_clacker_bomb, &pos, NULL, &effectScale);
                    fighter->toDead(-1);
                }
            }
        }
    }
}