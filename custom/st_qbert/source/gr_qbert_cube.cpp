#include <memory.h>
#include <ec/ec_mgr.h>
#include <so/so_external_value_accesser.h>
#include "gr_qbert_cube.h"
#include <mt/mt_prng.h>
#include <OS/OSError.h>
#include <ft/ft_manager.h>

grQbertCube* grQbertCube::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grQbertCube* ground = new(Heaps::StageInstance) grQbertCube(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grQbertCube::update(float frameDelta){
    for (int team = 0; team < NUM_TEAMS; team++) {
        if (this->numMembersOnTeamLanded[team] > this->prevNumMembersOnTeamLanded[team]) {
            this->setTeam(team, true);
        }
        this->prevNumMembersOnTeamLanded[team] = this->numMembersOnTeamLanded[team];
        this->numMembersOnTeamLanded[team] = 0;
    }

    if (this->timer > 0) {
        if (this->timer - frameDelta <= 0) {
            this->timer = 0;
            this->setTeam(0, false);
        }
        else {
            this->timer -= frameDelta;
        }
    }

}

void grQbertCube::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    if (this->timer <= 0) {
        gfTask* stageObject = gfTask::getTask(collStatus->m_taskId);
        int teamNumber = soExternalValueAccesser::getTeamNo(static_cast<StageObject*>(stageObject));
        CollCategoryFlag categoryFlag(COLL_CATEGORY_MASK_FIGHTER);
        int entryId = -1;
        if (this->isCollisionStatusOwnerTask(collStatus, &categoryFlag)) {
            entryId = g_ftManager->getEntryIdFromTaskId(collStatus->m_taskId, NULL);
            if (entryId > -1) {
                teamNumber = g_ftManager->getTeam(entryId, false, false);
            }
        }
        if (teamNumber >= 0 && teamNumber < NUM_TEAMS - 1) {
            teamNumber++;
        }
        else {
            teamNumber = DEFAULT_TEAM_ID;
        }
        this->numMembersOnTeamLanded[teamNumber]++;
        if (entryId > -1) {
            this->lastLanderEntryIdForTeamCube[teamNumber] = entryId;
            this->lastLanderEntryIdForTeamWork[teamNumber] = entryId;
        }
    }
}

u32 grQbertCube::getNumNextJumpCubes() {
    return this->m_sceneModels[0]->m_resMdl.GetResNodeNumEntries() - this->getNodeIndex(0, "Jumps") - 1;
}

void grQbertCube::getNextJumpCubes(u32* cubeIndices) {
    u32 numNodes = this->m_sceneModels[0]->m_resMdl.GetResNodeNumEntries();
    u32 startJumpNodeIndex = this->getNodeIndex(0, "Jumps") + 1;
    for (u32 i = startJumpNodeIndex; i < numNodes; i++) {
        Vec3f scale;
        this->getNodeScale(&scale, 0, i);
        cubeIndices[i - startJumpNodeIndex] = scale.m_x; // use scale.x as index for next cube
    }
}

void grQbertCube::setNumBlocksPerTeamWork(u8* numBlocksPerTeam) {
    this->numBlocksPerTeam = numBlocksPerTeam;
}

void grQbertCube::setLastLanderEntryIdForTeamWork(int* lastLanderEntryIdForTeamWork) {
    this->lastLanderEntryIdForTeamWork = lastLanderEntryIdForTeamWork;
}

u8 grQbertCube::getTeam() {
    return this->teamId;
}

void grQbertCube::setTeam(u8 teamId, bool incrementScore) {
    if (this->timer <= 0) {
        if (teamId < 0 || teamId >= NUM_TEAMS) {
            teamId = DEFAULT_TEAM_ID;
        }

        this->setMotionDetails(0, 0, teamId, 0, 0);
        if (this->teamId != teamId) {
            this->numBlocksPerTeam[this->teamId]--;
            this->numBlocksPerTeam[teamId]++;
            if (teamId > 0) {
                this->soundGenerator.playSE(snd_se_stage_Madein_10, 0x0, 0x0, 0xffffffff);
                if (teamId - 1 < NUM_PLAYERS && incrementScore) {
                    this->teamScoresWork[teamId - 1] += CUBE_POINTS;
                    if (this->gameRule == Game_Rule_Coin) {
                        if (this->lastLanderEntryIdForTeamCube[teamId] > -1) {
                            g_ftManager->pickupCoin(this->lastLanderEntryIdForTeamCube[teamId], CUBE_POINTS);
                        }
                        else if (this->lastLanderEntryIdForTeamWork[teamId] > -1) {
                            g_ftManager->pickupCoin(this->lastLanderEntryIdForTeamWork[teamId], CUBE_POINTS);
                        }
                    }
                }
            }
        }
        this->teamId = teamId;
    }
};

void grQbertCube::setTeamScoresWork(u32* teamScoresWork, GameRule gameRule) {
    this->teamScoresWork = teamScoresWork;
    this->gameRule = gameRule;
}

void grQbertCube::setWin() {
    this->setTeam(0, false);
    this->setMotionDetails(0, 0, NUM_TEAMS, 0, 0);
    stQbertStageData* qbertStageData = (stQbertStageData*)this->getStageData();
    this->timer = qbertStageData->winFrames;
}
