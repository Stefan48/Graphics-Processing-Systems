#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/particle_effect.h"

#include <string>


namespace m2
{
    struct Pawn
    {
        int id;
        int line, column;
        bool isWhite;
        bool isHighlighted;
        bool isActive;

        Pawn() {}

        Pawn(int id, int line, int column, bool isWhite)
        {
            this->id = id;
            this->line = line;
            this->column = column;
            this->isWhite = isWhite;
            this->isHighlighted = false;
            this->isActive = true;
        }
    };

    struct Square
    {
        int id;
        float x, z;
        bool isWhite;
        bool isHighlighted;
        Pawn *pawn;

        Square() {}

        Square(int id, float x, float z, bool isWhite)
        {
            this->id = id;
            this->x = x;
            this->z = z;
            this->isWhite = isWhite;
            this->isHighlighted = false;
            this->pawn = NULL;
        }
    };

    struct Particle
    {
        glm::vec4 position;
        glm::vec4 speed;

        Particle() {}

        Particle(const glm::vec4 &position, const glm::vec4 &speed)
        {
            SetParams(position, speed);
        }

        void SetParams(const glm::vec4 &position, const glm::vec4 &speed)
        {
            this->position = position;
            this->speed = speed;
        }
    };
    

    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        unsigned int UploadCubeMapTexture(const std::string &pos_x, const std::string &pos_y, const std::string &pos_z, const std::string &neg_x, const std::string &neg_y, const std::string &neg_z);
        void RenderMeshInstanced(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, int instances, const glm::vec3 &color = glm::vec3(1));
        void ResetParticleSystem(glm::vec3 capturePosition);

        std::vector<std::pair<int, int>> GetPossibleMoves(Pawn *pawn);
        void HighlightPossibleMoves(Pawn *pawn);
        void UnhighlightPossibleMoves(Pawn *pawn);
        void UpdateMovingPawn(float deltaTime);


    private:
        int cubeMapTextureID;

        bool noColor = false;
        glm::vec3 white = glm::vec3(1, 1, 1);
        glm::vec3 black = glm::vec3(0, 0, 0);
        glm::vec3 green = glm::vec3(0, 1, 0);

        bool isWhiteTurn = true;

        Square board[8][8];
        float squareLength;

        Pawn pawns[2][12];
        float pawnScale;
        Pawn *highlightedPawn = NULL;
        Pawn *movingPawn = NULL;
        // parameters for pawn movement
        float jumpStepHorizontal = 1.0f, jumpStepVertical = 1.25f;
        glm::vec3 currentPos, destPos;
        float jumpDistance;

        // parameters for generated curves
        glm::vec3 control_p0[2][3], control_p1[2][3], control_p2[2][3], control_p3[2][3];
        int numCurveInstances;
        int numGeneratedPoints;

        // particle system
        ParticleEffect<Particle> *particleEffect;
        bool particlesActive = false;
        float particlesTimer;
        float particlesTimerLimit = 1.0f;
        float particlesTimerStep = 0.5f;
        int numParticles = 1000;
        glm::vec4 particleSpeeds[1000];
    };
}   // namespace m2
