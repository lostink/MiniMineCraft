#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QSound>
#include <QDir>
#include <proceduralterrain.h>

class AudioManager
{
public:
    AudioManager();
    void stopPlay(QSound* sound);
    QSound* playFootstep(biometype type, int y);
    QSound* playDamage();
    QSound* playPlace();
    QSound* playWater();
    //void playWind();
    QSound *grassStep;
    QSound *sandStep;
    QSound *snowStep;
    QSound *stoneStep;
    QSound *damage;
    QSound *place;
    QSound *water;
};

#endif // AUDIOMANAGER_H
