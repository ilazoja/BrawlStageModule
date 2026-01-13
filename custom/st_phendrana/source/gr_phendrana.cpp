#include "gr_phendrana.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grPhendrana* grPhendrana::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grPhendrana* ground = new (Heaps::StageInstance) grPhendrana(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grPhendrana::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType)
{
    grYakumono::startup(archive, unk1, layerType);
}

void grPhendrana::update(float deltaFrame)
{
    grYakumono::update(deltaFrame);
}