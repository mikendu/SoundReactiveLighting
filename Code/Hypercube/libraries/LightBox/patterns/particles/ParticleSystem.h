#ifndef PARTICLE_BUFFER_H
#define PARTICLE_BUFFER_H

#define MAX_PARTICLES 32

#include "LightParticle.h"
#include "NeopixelStrip.h"
#include "SpectrumAnalyzer.h"
#include "LinkedList.h"
#include "ParticleEmitter.h"

class ParticleSystem
{

public:
    ParticleSystem() 
    {
        for(uint8_t i = 0; i < MAX_PARTICLES; i++) 
            expiredParticles.add(&buffer[i]);
    }
    
    void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
        BufferNode<LightParticle>* currentNode = liveParticles.first();
        while (currentNode != nullptr) 
        {
            LightParticle* particle = currentNode->item;
            particle->updateSoundParameters(spectrum, parameters);
            currentNode = currentNode->next;
        }

        for (ParticleEmitter* emitter : emitters)
            emitter->updateSoundParameters(spectrum, parameters);
    }

    void update(float elapsedSeconds, uint16_t stripLength) 
    {
        for (ParticleEmitter* emitter : emitters)
            emitter->update(this, elapsedSeconds, stripLength);

        BufferNode<LightParticle>* currentNode = liveParticles.first();
        while (currentNode != nullptr) 
        {
            LightParticle* particle = currentNode->item;
            bool alive = particle->update(elapsedSeconds, stripLength);
            if (!alive) 
            {
                BufferNode<LightParticle>* expiredNode = currentNode;
                currentNode = currentNode->next;
                liveParticles.remove(expiredNode);
                expiredParticles.add(expiredNode);
            } 
            else 
            {
                currentNode = currentNode->next;
            }
        }
    }   

    void show(NeopixelStrip& strip, ColorParameters& parameters)
    {
        BufferNode<LightParticle>* currentNode = liveParticles.first();
        while (currentNode != nullptr) 
        {
            LightParticle* particle = currentNode->item;
            particle->show(strip, parameters);
            currentNode = currentNode->next;
        }
    }

    bool spawn(ParticleProperties properties, float location) 
    {
        if (expiredParticles.size() > 0) 
        {
            BufferNode<LightParticle>* node = expiredParticles.last();
            LightParticle* particle = node->item;
            particle->spawn(properties, location);

            expiredParticles.remove(node);
            liveParticles.add(node);
            return true;
        }
        return false;
    }

    void addEmitter(ParticleEmitter* emitter) 
    {
        emitters.push_back(emitter);
    }

private:

    LightParticle buffer[MAX_PARTICLES];
    LinkedList<LightParticle> liveParticles;
    LinkedList<LightParticle> expiredParticles;
    std::vector<ParticleEmitter*> emitters;

};

#endif