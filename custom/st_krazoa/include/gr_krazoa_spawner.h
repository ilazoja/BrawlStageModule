#pragma once

#include <gr/gr_madein.h>
#include <st/st_melee.h>

class grKrazoaSpawner : public grMadein
{

    protected:
        float timer;
        grGimmickMotionPathData motionPathData;
        float minRespawnFrames;
        float maxRespawnFrames;
        float spawnAfterFrames;
        bool isSilentDeployment;

    public:
        grKrazoaSpawner(const char* taskName) : grMadein(taskName) {
            timer = 0;
            isSilentDeployment = true;
        };
        virtual ~grKrazoaSpawner() { };
        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        virtual void setConfig(int motionPathMdlIndex, float minRespawnFrames, float maxRespawnFrames, float spawnAfterFrames, bool isFirstSilentDeployment);

        static grKrazoaSpawner* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
