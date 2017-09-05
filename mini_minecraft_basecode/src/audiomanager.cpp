#include "audiomanager.h"


AudioManager::AudioManager()
{
    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cd("mini_minecraft_basecode/audio");
    grassStep = new QSound(currentDir.path()+"/grassStep.wav");
//    grassStep->setLoops(QSound::Infinite);
    sandStep = new QSound(currentDir.path()+"/sandStep.wav");
//    sandStep->setLoops(QSound::Infinite);
    snowStep = new QSound(currentDir.path()+"/snowStep.wav");
//    snowStep->setLoops(QSound::Infinite);
    stoneStep = new QSound(currentDir.path()+"/stoneStep.wav");
//    stoneStep->setLoops(QSound::Infinite);
    water = new QSound(currentDir.path()+"/water.wav");
    place = new QSound(currentDir.path()+"/place.wav");
    damage = new QSound(currentDir.path()+"/damage.wav");
}


void AudioManager::stopPlay(QSound* sound)
{
    sound->stop();
}


QSound* AudioManager::playFootstep(biometype type, int y)
{


    if(y >= 0)
    {
        if((type == PLAIN)||(type == FOREST)||(type == SWAMP))
        {

            grassStep->play();
            return grassStep;
        }
        else if(type == DESERT)
        {
            sandStep->play();
            return sandStep;
        }
        else if(type == SNOW)
        {

            snowStep->play();
            return snowStep;
        }
    }
    else
    {
        stoneStep->play();
        return stoneStep;
    }

}

QSound* AudioManager::playDamage()
{
    //damage->setLoops(QSound::Infinite);
    damage->play();
    return damage;
}

QSound* AudioManager::playPlace()
{
    //place->setLoops(QSound::Infinite);
    place->play();
    return place;
}

QSound* AudioManager::playWater()
{
    //water->setLoops(QSound::Infinite);
    //water->play();
    return water;
}
