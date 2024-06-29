#pragma once
#include "layer.h"

#include <glew.h>
#include <glm/glm.hpp>

#include <map>
#include <list>

class CProgram;
class CMesh;

struct TexturedParticleType
{
    GLuint TextureID;

    float LocationVariation = 0.0f;

    glm::vec2 VelocityCenter;
    float VelocityVariation = 0.0f;

    glm::vec2 AccelerationCenter;
    float AccelerationVariation = 0.0f;

    float ScaleCenter = 0.0f;
    float ScaleVariation = 0.0f;

    float LifeTimeCenter = 0.0f;
    float LifeTimeVariation = 0.0f;
};

class CTexturedParticleLayer :
    public ILayer
{
public:
    CTexturedParticleLayer();

    void Update() override;
    void Render() override;

    void AddParticleType(std::string name, TexturedParticleType type);

    void SpawnBurst(glm::vec2 location, std::string particleType, int count);
    void SpawnParticle(glm::vec2 location, std::string particleType);

private:
    CProgram* TextureProgram;
    CMesh* SquareMesh;

    struct TexturedParticle
    {
        glm::vec2 Location;
        glm::vec2 Velocity;
        glm::vec2 Acceleration;

        float OriginalScale;
        float Scale;

        float OriginalLifeTime;
        float LifeTime;

        GLuint TextureID;
    };

    std::list<TexturedParticle> Particles;
    std::map<std::string, TexturedParticleType> ParticleTypes;


    void SpawnSpecificParticle(glm::vec2 location, TexturedParticleType type);
};

