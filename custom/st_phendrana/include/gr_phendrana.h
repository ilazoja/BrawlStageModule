#pragma once

#include <gr/gr_yakumono.h>

class grPhendrana : public grYakumono
{
    public:
        grPhendrana(const char* taskName) : grYakumono(taskName) {};
        virtual ~grPhendrana(){};

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        static grPhendrana* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};