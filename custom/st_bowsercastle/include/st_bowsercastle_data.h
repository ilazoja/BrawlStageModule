#pragma once

#include <types.h>
#include <StaticAssert.h>
#include <st/st_collision_attr_param.h>
#include <snd/snd_id.h>

//const SndID snd_se_stage_BowserCastle_Thwomp_Spawn = snd_se_stage_Pirates_07;
//const SndID snd_se_stage_BowserCastle_Thwomp_Shake = snd_se_stage_Pirates_18;
//const SndID snd_se_stage_BowserCastle_Thwomp_Land = snd_se_stage_Pirates_02;
//const SndID snd_se_stage_BowserCastle_Thwomp_Stomp = snd_se_stage_Pirates_16;
//const SndID snd_se_stage_BowserCastle_Brick_Break = snd_se_stage_Pirates_19;
//const SndID snd_se_stage_BowserCastle_Brick_Spawn = snd_se_stage_Pirates_06;
//const SndID snd_se_stage_BowserCastle_Podoboo_Hop = snd_se_stage_Pirates_04;
//const SndID snd_se_stage_BowserCastle_Podoboo_Sink = snd_se_stage_Pirates_05;
//const SndID snd_se_stage_BowserCastle_Ambience = snd_se_stage_Pirates_03;
//const SndID snd_se_stage_BowserCastle_Laugh = snd_se_stage_Pirates_01;

const int kStageSoundbankID = 0x201;
const int kNumPodobooSfxVariation = 3;
const SndID snd_se_stage_BowserCastle_Thwomp_Spawn = 0x402F + ((kStageSoundbankID - 0x144) * 0xA5);
const SndID snd_se_stage_BowserCastle_Thwomp_Shake = snd_se_stage_BowserCastle_Thwomp_Spawn + 1;
const SndID snd_se_stage_BowserCastle_Thwomp_Land = snd_se_stage_BowserCastle_Thwomp_Shake + 1;
const SndID snd_se_stage_BowserCastle_Thwomp_Stomp = snd_se_stage_BowserCastle_Thwomp_Land + 1;
const SndID snd_se_stage_BowserCastle_Brick_Break = snd_se_stage_BowserCastle_Thwomp_Stomp + 1;
const SndID snd_se_stage_BowserCastle_Brick_Spawn = snd_se_stage_BowserCastle_Brick_Break + 1;
const SndID snd_se_stage_BowserCastle_Ambience = snd_se_stage_BowserCastle_Brick_Spawn + 1;
const SndID snd_se_stage_BowserCastle_Laugh = snd_se_stage_BowserCastle_Ambience + 1;
const SndID snd_se_stage_BowserCastle_Podoboo_Hop = snd_se_stage_BowserCastle_Laugh + 1;
const SndID snd_se_stage_BowserCastle_Podoboo_Sink = snd_se_stage_BowserCastle_Podoboo_Hop + kNumPodobooSfxVariation;

struct stBowserCastleData {
    grGimmickDamageFloor damageFloor;
    u32 thwompMinActivateFrames;
    u32 thwompMaxActivateFrames;
    u32 thwompShakeActivateFrames;
    u32 thwompMinRespawnFrames;
    u32 thwompMaxRespawnFrames;
    u32 brickMinRespawnFrames;
    u32 brickMaxRespawnFrames;
    u32 laughStartFrames;
    u32 laughCooldownFrames;
    Vec2f thwompSpawnZoomOut;
    u32 podobooMinRespawnFrames;
    u32 podobooMaxRespawnFrames;
    u32 podobooSinkFrame;
    u32 thwompEffectDropFrame;
};