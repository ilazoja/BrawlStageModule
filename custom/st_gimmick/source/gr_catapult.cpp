#pragma once

#include "gr_catapult.h"
#include <memory.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>

grCatapult* grCatapult::create(int mdlIndex, const char* taskName) {
    grCatapult* catapult = new (Heaps::StageInstance) grCatapult(taskName);
    catapult->setMdlIndex(mdlIndex);
    return catapult;
}

void grCatapult::prepareCatapultData(float vector, float motionRatio, int motionPathIndex, float framesBeforeStartMove, float unk1, float unk2) {
    Vec2f areaPos = Vec2f(0.0, 3.0);
    Vec2f areaRange = Vec2f(10.0, 5.0);

    this->_catapultData.initialize(
            framesBeforeStartMove, unk1, unk2, vector, this->m_mdlIndex, (vector > 90 && vector <= 270) ? true : false, true,
            motionRatio, 0, motionPathIndex, 0,
            &areaPos, &areaRange
    );
    this->setGimmickData(&this->_catapultData);
}