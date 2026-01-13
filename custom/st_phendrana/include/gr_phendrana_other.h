#pragma once

#include "gr_phendrana_item.h"
#include <st/st_melee.h>

class grPhendranaOther : public grPhendranaItem
{
    public:
        grPhendranaOther(const char* taskName) : grPhendranaItem(taskName) {};
        virtual ~grPhendranaOther() { };

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);

        static grPhendranaOther* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};