#include "gr_final.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <ft/ft_external_value_accesser.h>
#include <OS/OSError.h>


grFinal* grFinal::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grFinal* ground = new (Heaps::StageInstance) grFinal(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grFinal::update(float deltaFrame)
{

}

void grFinal::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    gfTask* stageObject = gfTask::getTask(collStatus->m_taskId);
    int unk = 1;
    if (this->isCollisionStatusOwnerTask(collStatus, &unk)) {
        Fighter* fighter = dynamic_cast<Fighter*>(stageObject);
        if (fighter->m_moduleAccesser->getStatusModule()->getStatusKind() == 0x04) {
            Vec3f pos = ftExternalValueAccesser::getHipPos(fighter);
            g_ecMgr->setEffect(0x330001, &pos);

        }

    }

}
