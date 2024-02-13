#pragma once

#include "components/simple_scene.h"

#define MAX_PLAYER_STARS 14


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void LoadHexagon(std::string meshName, glm::vec3 color);
        void LoadStar(std::string meshName, glm::vec3 color);
        void FrameStart() override;
        void RenderShooter(float x, float y, std::string type, float scaleFactor);
        void RenderHexagon(float x, float y, std::string type, float scaleFactor);
        void RenderStar(float x, float y, std::string type, float angle);
        void RenderGUI(float deltaTimeSeconds);
        void RenderProjectiles(float deltaTimeSeconds);
        void RenderPickups(float deltaTimeSeconds);
        void RenderEnemies(float deltaTimeSeconds);
        bool SameColor(std::string name1, std::string name2);
        void DetectCollisions();
        void RenderGame(float deltaTimeSeconds);
        void RenderGameOver(float deltaTimeSeconds);
        void Update(float deltaTimeSeconds) override;

        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        bool IsInTile(int mouseX, int mouseY, int i, int j);
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

     protected:
         struct Tile {
             glm::vec2 position;
             bool isUsed;
             std::string usedType;
             bool isDying;
             float scaleFactor;
             bool attackedByOrange;
             bool attackedByBlue;
             bool attackedByYellow;
             bool attackedByPurple;
             float timer;
        } tiles[3][3];
        struct Hexagon {
            glm::vec2 position;
            int healthPoints;
            int line;
            std::string type;
            bool isDying;
            float scaleFactor;
        };
        struct Star {
            glm::vec2 position;
            float rotation;
            int line;
            std::string type;
        };
        float cx, cy;
        glm::mat4 modelMatrix;
        float borderSpacing;
        float tileSpacing;
        float shooterSpacing;
        float squareSide;
        float homeScaleX, homeScaleY;
        float shooterBodyScaleX, shooterBodyScaleY;
        float shooterGunScaleX, shooterGunScaleY;
        float homeCenter;
        glm::vec3 orange, blue, yellow, purple, grey;
        int playerLives;
        int playerStars;
        const int maxPlayerStars = 5;
        bool selectedShooter;
        std::string selectedType;
        int mouseX, mouseY;
        float pickupTimer;
        float enemySpawnTimer;
        std::vector<glm::ivec2> pickupPositions;
        std::vector<Hexagon> enemies;
        std::vector<Star> projectiles;
        float skullRotation;
    };
}   // namespace m1
