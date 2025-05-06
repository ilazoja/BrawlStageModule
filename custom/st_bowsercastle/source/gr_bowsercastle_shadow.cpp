#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_bowsercastle_shadow.h"
#include <OS/OSError.h>

grBowserCastleShadow* grBowserCastleShadow::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastleShadow* shadow = new(Heaps::StageInstance) grBowserCastleShadow(taskName);
    shadow->setupMelee();
    shadow->setMdlIndex(mdlIndex);
    shadow->m_heapType = Heaps::StageInstance;
    shadow->makeCalcuCallback(1, Heaps::StageInstance);
    shadow->setCalcuCallbackRoot(7);

    return shadow;
}

void grBowserCastleShadow::update(float deltaFrame){
    grMadein::update(deltaFrame);
}

void grBowserCastleShadow::changeState(State state) {
    if (this->state != state) {
        this->setMotion(state);
        this->state = state;
    }
}
