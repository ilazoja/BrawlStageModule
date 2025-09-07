#pragma once

#include <gr/gr_yakumono.h>
#include "st_qbert_stage_data.h"

class grQbertScore : public grYakumono
{
    protected:
        Vec3f scaleBase;
        Vec3f* posWork;
        u32* scoreWork;
        u32 prevScore;
        u8 type;
    public:
        grQbertScore(const char* taskName) : grYakumono(taskName) {
            prevScore = 999999;
            m_motionRatio = 0.0;
            scaleBase = Vec3f(0.0, 0.0, 0.0);
            if (&m_calcWorldCallBack != NULL) {
                m_calcWorldCallBack.m_numNodeCallbackData = 1;
                m_calcWorldCallBack.initialize(false, Heaps::StageInstance);
                m_calcWorldCallBack.m_nodeCallbackDatas[0].m_flag0 = true;
                m_calcWorldCallBack.m_nodeCallbackDatas[0].m_flag2 = true;
                this->setupMelee();
            }

        };
        virtual void update(float frameDelta);
        virtual ~grQbertScore() { };
        virtual void setPosWork(Vec3f* posWork);
        virtual void setScoreWork(u32* scoreWork);
        virtual void setType(u8 type);
        virtual void updateScaleBase(float frameDelta);
        virtual void updateCallback(float frameDelta);
        virtual void updateNumber(float frameDelta);

        static grQbertScore* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
