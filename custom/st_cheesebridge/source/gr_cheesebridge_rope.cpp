#include "gr_cheesebridge_rope.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grCheeseBridgeRope* grCheeseBridgeRope::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grCheeseBridgeRope* rope = new (Heaps::StageInstance) grCheeseBridgeRope(taskName);
    rope->setMdlIndex(mdlIndex);
    rope->m_heapType = Heaps::StageInstance;
    rope->makeCalcuCallback(1, Heaps::StageInstance);
    rope->setCalcuCallbackRoot(7);
    rope->setupMelee();

    return rope;
}

void grCheeseBridgeRope::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grCheeseBridgePlatform::startup(archive, unk1, unk2);
    this->m_soundEffects[0].m_id = snd_se_stage_Jungle_01;

    stCheeseBridgeData* stageData = static_cast<stCheeseBridgeData*>(this->getStageData());
    this->railParam = &stageData->ropeParam;
    this->respawnTimer = randf()*(railParam->maxRespawnFrames - railParam->minRespawnFrames) + railParam->minRespawnFrames;

    Vec3f upperNode;
    Vec3f underNode;
    this->getUpperNode(&upperNode);
    this->getUnderNode(&underNode);
    Vec3f rootNode;
    int root = 0;
    this->getNodePosition(&rootNode, 0, root);
    this->m_areaData.set(gfArea::Shape_Rectangle, gfArea::Stage_Group_Gimmick_Ladder, 0, 0, 0, 0, Vec2f(0.0, (upperNode.m_y + underNode.m_y)/2), Vec2f(10.0, upperNode.m_y - underNode.m_y));
    this->setAreaGimmick(&this->m_areaData, &this->m_areaDataSet, &this->m_ykData, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Ladder, -1);
    trigger->setObserveYakumono(this->m_yakumono);

}

void grCheeseBridgeRope::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    soGimmickLadderEventArgs* ladderEventInfo = static_cast<soGimmickLadderEventArgs*>(eventInfo);
    switch(ladderEventInfo->m_kind) {
        case 0x14:
            this->getUpperNode(&ladderEventInfo->m_upperNodePos);
            this->getUnderNode(&ladderEventInfo->m_underNodePos);
            ladderEventInfo->m_restrictUpExit = true;
            ladderEventInfo->m_33 = true;
            break;
        default:
            break;
    }
}

void grCheeseBridgeRope::getUpperNode(Vec3f* pos) {
    this->getNodePosition(pos, 0, "upper");
}

void grCheeseBridgeRope::getUnderNode(Vec3f* pos) {
    this->getNodePosition(pos, 0, "under");
}

