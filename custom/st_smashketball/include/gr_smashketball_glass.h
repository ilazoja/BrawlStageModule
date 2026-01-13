#pragma once

#include <gr/gr_madein.h>

class grSmashketballGlass : public grMadein {
    protected:
        ykData yakumonoData;
        float timer;

    public:
        grSmashketballGlass(const char* taskName) : grMadein(taskName), yakumonoData(1, NULL)
        {
            timer = 0;
        };
        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);
        virtual ~grSmashketballGlass(){};
        virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

        virtual void setupHitPoint();
        virtual void setHit();

        static grSmashketballGlass* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};

