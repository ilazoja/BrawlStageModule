#include <memory.h>
#include <ec_mgr.h>
#include <so/so_external_value_accesser.h>
#include "gr_qbert.h"
#include <OSError.h>

grQbert* grQbert::create(int mdlIndex, char* tgtNodeName, char* taskName){
    grQbert* ground = new(StageInstance) grQbert(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->heapType = StageInstance;
    ground->makeCalcuCallback(1, StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grQbert::update(float frameDiff){
    for (int team = 0; team < NUM_TEAMS; team++) {
        if (this->numMembersOnTeamLanded[team] > this->prevNumMembersOnTeamLanded[team]) {
            this->setMotionDetails(0, 0, team, 0, 0);
        }
        this->prevNumMembersOnTeamLanded[team] = this->numMembersOnTeamLanded[team];
        this->numMembersOnTeamLanded[team] = 0;
    }

    //
}

void grQbert::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    gfTask* stageObject = gfTask::getTask(collStatus->taskId);
    int teamNumber = soExternalValueAccesser::getTeamNo((StageObject*)stageObject);
    if (teamNumber >= 0 || teamNumber < NUM_TEAMS - 1) {
        teamNumber++;
    }
    else {
        teamNumber = 0;
    }
    this->numMembersOnTeamLanded[teamNumber]++;


}

// TODO: Hit qbert to change him into your colour