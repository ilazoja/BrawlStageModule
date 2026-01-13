#pragma once

#include <gr/gr_madein.h>

class grIzumiStar : public grMadein {
protected:
    int lastPathIndex;
    float timer;

public:
    grIzumiStar(const char* taskName) : grMadein(taskName)
    {
        lastPathIndex = -1;
        timer = 0;
    };
    virtual void update(float deltaFrame);
    virtual ~grIzumiStar(){};
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setTimer();
    virtual void startStarEffect(u32 colourIndex);
    virtual void stopStarEffect();

    static grIzumiStar* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
