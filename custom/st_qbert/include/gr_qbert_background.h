#pragma once

#include <gr/gr_madein.h>
#include "st_qbert_stage_data.h"

class grQbertBackground : public grMadein
{
    protected:
        float immobilizeTimer;
    public:
        grQbertBackground(const char* taskName) : grMadein(taskName) {
            immobilizeTimer = 0;
        };
        virtual void update(float frameDiff);
        virtual ~grQbertBackground() { };
        virtual void setImmobilize(float immobilizeDuration);

        static grQbertBackground* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
