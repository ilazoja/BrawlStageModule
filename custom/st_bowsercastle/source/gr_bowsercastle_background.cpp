#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_bowsercastle_background.h"
#include <OS/OSError.h>

grBowserCastleBackground* grBowserCastleBackground::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastleBackground* bg = new(Heaps::StageInstance) grBowserCastleBackground(taskName);
    bg->setupMelee();
    bg->setMdlIndex(mdlIndex);
    bg->setTgtNode(tgtNodeName);

    return bg;
}

