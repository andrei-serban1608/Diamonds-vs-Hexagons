#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/lab2/lab2.h"
#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

void Tema1::LoadHexagon(std::string meshName, glm::vec3 color)
{
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    for (int i = 0; i < 6; i++) {
        vertices.push_back(VertexFormat(glm::vec3(cos((2 * i * M_PI) / 6), sin((2 * i * M_PI) / 6), 0), color));
    }

    for (int i = 1; i <= 5; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(6);
    indices.push_back(1);
    meshes[meshName] = new Mesh(meshName);
    meshes[meshName]->InitFromData(vertices, indices);
}

void Tema1::LoadStar(std::string meshName, glm::vec3 color)
{
    vector<VertexFormat> vertices;
    // If phi is the golden ratio, the ratio between the big and small radius of the star is 1 + phi
    // Computing the value for 1 / (1 + phi)
    double radiusRatio = .3819660112501;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    for (int i = 0; i < 5; i++) {
        vertices.push_back(VertexFormat(glm::vec3(cos((2 * i * M_PI) / 5), sin((2 * i * M_PI) / 5), 0), color));
        vertices.push_back(VertexFormat(glm::vec3(radiusRatio * -cos((2 * i * M_PI) / 5), radiusRatio * -sin((2 * i * M_PI) / 5), 0), color));
    }

    vector<unsigned int> indices =
    {
        1, 5, 4,
        3, 7, 6,
        5, 9, 8
    };

    meshes[meshName] = new Mesh(meshName);
    meshes[meshName]->InitFromData(vertices, indices);
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    squareSide = 100;

    cx = (corner.x + squareSide) / 2;
    cy = (corner.y + squareSide) / 2;

    borderSpacing = 10.f;
    tileSpacing = 20.f;
    shooterSpacing = 90.f;
    homeScaleX = 0.5f;
    homeScaleY = (float)(3 * squareSide + 2 * tileSpacing) / squareSide;
    homeCenter = borderSpacing + (homeScaleX * squareSide) / 2;
    shooterBodyScaleX = 0.33f;
    shooterBodyScaleY = 1.f / sqrt(2);
    shooterGunScaleX = 0.4f;
    shooterGunScaleY = 0.15f;
    mouseX = 0;
    mouseY = 0;

    orange = glm::vec3((float)0xFF / 256, (float)0x57 / 256, (float)0x00 / 256);
    blue = glm::vec3((float)0x1D / 256, (float)0xA1 / 256, (float)0xF2 / 256);
    yellow = glm::vec3((float)0xFF / 256, (float)0xFC / 256, (float)0x00 / 256);
    purple = glm::vec3((float)0x91 / 256, (float)0x46 / 256, (float)0xFF / 256);
    grey = glm::vec3((float)0xA3 / 256, (float)0xA3 / 256, (float)0xA3 / 256);

    // player starts out with 3 lives
    playerLives = 3;
    // player starts out with 5 stars
    playerStars = 5;
    
    selectedShooter = false;
    pickupTimer = 1;
    enemySpawnTimer = 5;

    // Home bar square
    Mesh* home = object2D::CreateSquare("home", corner, squareSide, glm::vec3((float)0xC5 / 256, (float)0x11 / 256, (float)0x04 / 256), true);
    AddMeshToList(home);

    // Tile squares
    Mesh* tile = object2D::CreateSquare("tile", corner, squareSide, glm::vec3((float)0x26 / 256, (float)0xB1 / 256, (float)0x70 / 256), true);
    AddMeshToList(tile);

    // Shooter squares
    Mesh* shooterOrange = object2D::CreateSquare("shooterOrange", corner, squareSide, orange, true);
    AddMeshToList(shooterOrange);
    Mesh* shooterBlue = object2D::CreateSquare("shooterBlue", corner, squareSide, blue, true);
    AddMeshToList(shooterBlue);
    Mesh* shooterYellow = object2D::CreateSquare("shooterYellow", corner, squareSide, yellow, true);
    AddMeshToList(shooterYellow);
    Mesh* shooterPurple = object2D::CreateSquare("shooterPurple", corner, squareSide, purple, true);
    AddMeshToList(shooterPurple);

    // Border GUI squares
    Mesh* border = object2D::CreateSquare("border", corner, squareSide, grey, false);
    AddMeshToList(border);

    // Hexagon enemies
    LoadHexagon("hexagonOrange", orange);
    LoadHexagon("hexagonBlue", blue);
    LoadHexagon("hexagonYellow", yellow);
    LoadHexagon("hexagonPurple", purple);
    LoadHexagon("hexagonInner", glm::vec3((float)0x40 / 256, (float)0xE0 / 256, (float)0xD0 / 256));

    // Star projectiles and pickups
    LoadStar("starOrange", orange);
    LoadStar("starBlue", blue);
    LoadStar("starYellow", yellow);
    LoadStar("starPurple", purple);
    LoadStar("starGUI", grey);
    LoadStar("starPickup", glm::vec3((float)0xFF / 256, (float)0xFF / 256, (float)0xFF / 256));

    // Positions for the centers of all the tiles + other tile initializations
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            tiles[i][j].position.x = borderSpacing + homeScaleX * squareSide + tileSpacing + j * (tileSpacing + squareSide) + cx;
            tiles[i][j].position.y = borderSpacing + i * (tileSpacing + squareSide) + cy;
            tiles[i][j].isUsed = false;
            tiles[i][j].isDying = false;
            tiles[i][j].scaleFactor = 1;
            tiles[i][j].attackedByOrange = false;
            tiles[i][j].attackedByBlue = false;
            tiles[i][j].attackedByYellow = false;
            tiles[i][j].attackedByPurple = false;
            tiles[i][j].timer = 1;
        }
    }

    // Renders skull for game over screen
    {
        Mesh* mesh = new Mesh("skull");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "12140_Skull_v3_L2.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    skullRotation = 0;
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor((float)0x04/256, (float)0x00/256, (float)0x20/256, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::RenderShooter(float x, float y, std::string type, float scaleFactor)
{
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(x, y, 0);
    modelMatrix *= transform3D::Scale(scaleFactor, scaleFactor, 0);
    modelMatrix *= transform3D::Scale(shooterBodyScaleX, shooterBodyScaleY, 0);
    modelMatrix *= transform3D::RotateOZ(RADIANS(45));
    modelMatrix *= transform3D::Translate(-cx, -cy, 0);
    RenderMesh(meshes[type], shaders["VertexColor"], modelMatrix);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(x, y, 0);
    modelMatrix *= transform3D::Scale(scaleFactor, scaleFactor, 0);
    modelMatrix *= transform3D::Translate(squareSide / 4, 0, 0);
    modelMatrix *= transform3D::Scale(shooterGunScaleX, shooterGunScaleY, 0);
    modelMatrix *= transform3D::Translate(-cx, -cy, 0);
    RenderMesh(meshes[type], shaders["VertexColor"], modelMatrix);
}

void Tema1::RenderHexagon(float x, float y, std::string type, float scaleFactor)
{
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(x, y, 0);
    modelMatrix *= transform3D::Scale(scaleFactor, scaleFactor, 0);
    modelMatrix *= transform3D::RotateOZ(RADIANS(45));
    modelMatrix *= transform3D::Scale(squareSide / 2, squareSide / 2, 0);
    RenderMesh(meshes[type], shaders["VertexColor"], modelMatrix);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(x, y, 1);
    modelMatrix *= transform3D::Scale(scaleFactor, scaleFactor, 0);
    modelMatrix *= transform3D::RotateOZ(RADIANS(45));
    modelMatrix *= transform3D::Scale(squareSide / 3, squareSide / 3, 0);
    RenderMesh(meshes["hexagonInner"], shaders["VertexColor"], modelMatrix);
}

void Tema1::RenderStar(float x, float y, std::string type, float angle)
{
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(x, y, 2);
    if (type == "starGUI") {
        modelMatrix *= transform3D::Translate(0, 0, -3);
        modelMatrix *= transform3D::Scale(squareSide / 8, squareSide / 8, 0);
    }
    else if (type == "starPickup") {
        modelMatrix *= transform3D::Scale(squareSide / 5, squareSide / 5, 0);
    }
    else {
        modelMatrix *= transform3D::Scale(squareSide / 2.5, squareSide / 2.5, 0);
        modelMatrix *= transform3D::RotateOZ(angle);
    }
    modelMatrix *= transform3D::RotateOZ(M_PI / 2);
    RenderMesh(meshes[type], shaders["VertexColor"], modelMatrix);
}

void Tema1::RenderGUI(float deltaTimeSeconds)
{
    // renders home bar as a scaled square
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(borderSpacing, borderSpacing, -1);
    modelMatrix *= transform3D::Scale(homeScaleX, homeScaleY, 0);
    RenderMesh(meshes["home"], shaders["VertexColor"], modelMatrix);

    // renders placement tiles
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelMatrix = glm::mat4(1);
            float translateX = borderSpacing + homeScaleX * squareSide + tileSpacing + j * (tileSpacing + squareSide);
            float translateY = borderSpacing + i * (tileSpacing + squareSide);
            modelMatrix *= transform3D::Translate(translateX, translateY, -1);
            RenderMesh(meshes["tile"], shaders["VertexColor"], modelMatrix);
            tiles[i][j].timer -= deltaTimeSeconds;
        }
    }

    // renders the bordered shooter types that the player can select from
    for (int i = 0; i < 4; i++) {
        modelMatrix = glm::mat4(1);
        float translateX = borderSpacing + i * (squareSide + shooterSpacing);
        float translateY = window->GetResolution().y - borderSpacing - squareSide;
        modelMatrix *= transform3D::Translate(translateX, translateY, -1);
        RenderMesh(meshes["border"], shaders["VertexColor"], modelMatrix);
        translateX += cx;
        translateY += cy;
        switch (i) {
        case 0:
            RenderShooter(translateX, translateY, "shooterOrange", 1);
            break;
        case 1:
            RenderShooter(translateX, translateY, "shooterBlue", 1);
            break;
        case 2:
            RenderShooter(translateX, translateY, "shooterYellow", 1);
            break;
        case 3:
            RenderShooter(translateX, translateY, "shooterPurple", 1);
            break;
        default:
            break;
        }
    }

    // renders the health points of the player
    for (int i = 0; i < playerLives; i++) {
        modelMatrix = glm::mat4(1);
        float translateX = window->GetResolution().x - borderSpacing - squareSide - (2 - i) * (tileSpacing + squareSide);
        float translateY = window->GetResolution().y - borderSpacing - squareSide;
        modelMatrix *= transform3D::Translate(translateX, translateY, -1);
        RenderMesh(meshes["home"], shaders["VertexColor"], modelMatrix);
    }

    // renders the star-cost of all the different types of shooters
    float starSpacing = 25.f;
    float starAllign = 10.f;
    for (int shooterNum = 1; shooterNum <= 4; shooterNum++) {
        for (int i = 0; i < shooterNum; i++) {
            float translateX = borderSpacing + (shooterNum - 1) * (squareSide + shooterSpacing) + starAllign + i * starSpacing;
            float translateY = window->GetResolution().y - borderSpacing - squareSide - 15.f;
            RenderStar(translateX, translateY, "starGUI", 0);
        }
    }

    // renders the current stars that the player has
    for (int i = 0; i < playerStars; i++) {
        float translateX = window->GetResolution().x - borderSpacing - squareSide - 2 * (tileSpacing + squareSide) + starAllign + i * starSpacing;
        float translateY = window->GetResolution().y - borderSpacing - squareSide - 15.f;
        RenderStar(translateX, translateY, "starGUI", 0);
    }
}

void Tema1::RenderProjectiles(float deltaTimeSeconds)
{
    // Starts rendering the stars that the active shooters shoot
    for (auto i = projectiles.begin(); i != projectiles.end(); ) {
        RenderStar(i->position.x, i->position.y, i->type, i->rotation);
        i->position.x += 400 * deltaTimeSeconds;
        i->rotation -= 2 * deltaTimeSeconds;
        if (i->position.x > window->GetResolution().x) {
            i = projectiles.erase(i);
        }
        else {
            i++;
        }
    }
}

void Tema1::RenderPickups(float deltaTimeSeconds)
{
    // Randomly renders pickups at a 7 second interval
    pickupTimer -= deltaTimeSeconds;
    if (pickupTimer < 0) {
        pickupTimer = 7;
        int verticalBound = window->GetResolution().y - borderSpacing - squareSide - 30;
        int horizontalBound = window->GetResolution().x;
        pickupPositions.push_back(glm::ivec2(rand() % horizontalBound, rand() % verticalBound));
        pickupPositions.push_back(glm::ivec2(rand() % horizontalBound, rand() % verticalBound));
        pickupPositions.push_back(glm::ivec2(rand() % horizontalBound, rand() % verticalBound));
    }

    for (int i = 0; i < pickupPositions.size(); i++) {
        RenderStar((float)pickupPositions[i].x, (float)pickupPositions[i].y, "starPickup", 0);
    }
}

void Tema1::RenderEnemies(float deltaTimeSeconds)
{
    // Randomly renders enemies at 5 second interval
    enemySpawnTimer -= deltaTimeSeconds;
    if (enemySpawnTimer < 0) {
        enemySpawnTimer = 5;
        Hexagon newEnemy;
        newEnemy.healthPoints = 3;
        newEnemy.isDying = false;
        newEnemy.scaleFactor = 1;
        newEnemy.line = rand() % 3;
        int typeNum = rand() % 4;
        switch (typeNum) {
        case 0:
            newEnemy.type = "hexagonOrange";
            break;
        case 1:
            newEnemy.type = "hexagonBlue";
            break;
        case 2:
            newEnemy.type = "hexagonYellow";
            break;
        case 3:
            newEnemy.type = "hexagonPurple";
            break;
        default:
            break;
        }
        float posX = window->GetResolution().x + squareSide / 4;
        float posY = borderSpacing + cy + newEnemy.line * (squareSide + tileSpacing);
        newEnemy.position = glm::vec2(posX, posY);
        enemies.push_back(newEnemy);
    }
    for (auto i = enemies.begin(); i != enemies.end(); ) {
        RenderHexagon(i->position.x, i->position.y, i->type, i->scaleFactor);
        i->position.x -= 80 * deltaTimeSeconds;

        if (i->isDying) {
            i->scaleFactor -= deltaTimeSeconds;

            // "Whenever you kill an enemy, you stop being attacked by it" - Socrates, probably
            if (i->type == "hexagonOrange") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByOrange = false;
                }
            }
            else if (i->type == "hexagonBlue") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByBlue = false;
                }
            }
            else if (i->type == "hexagonYellow") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByYellow = false;
                }
            }
            else if (i->type == "hexagonPurple") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByPurple = false;
                }
            }

            if (i->scaleFactor < 0.1f) {
                i = enemies.erase(i);
            }
            else {
                i++;
            }
        }
        else {
            if (i->type == "hexagonOrange") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByOrange = true;
                }
            }
            else if (i->type == "hexagonBlue") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByBlue = true;
                }
            }
            else if (i->type == "hexagonYellow") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByYellow = true;
                }
            }
            else if (i->type == "hexagonPurple") {
                for (int j = 0; j < 3; j++) {
                    tiles[i->line][j].attackedByPurple = true;
                }
            }

            if (i->position.x < homeCenter) {
                playerLives--;
                if (i->type == "hexagonOrange") {
                    for (int j = 0; j < 3; j++) {
                        tiles[i->line][j].attackedByOrange = false;
                    }
                }
                else if (i->type == "hexagonBlue") {
                    for (int j = 0; j < 3; j++) {
                        tiles[i->line][j].attackedByBlue = false;
                    }
                }
                else if (i->type == "hexagonYellow") {
                    for (int j = 0; j < 3; j++) {
                        tiles[i->line][j].attackedByYellow = false;
                    }
                }
                else if (i->type == "hexagonPurple") {
                    for (int j = 0; j < 3; j++) {
                        tiles[i->line][j].attackedByPurple = false;
                    }
                }
                i = enemies.erase(i);
            }
            else {
                i++;
            }
        }
    }
}

// Yeah I should've come up with some better standardization of the objects' colors...
bool Tema1::SameColor(std::string name1, std::string name2)
{
    return (name1 == "hexagonOrange" && name2 == "starOrange")
        || (name1 == "hexagonBlue"   && name2 == "starBlue")
        || (name1 == "hexagonYellow" && name2 == "starYellow")
        || (name1 == "hexagonPurple" && name2 == "starPurple");
}

void Tema1::DetectCollisions()
{
    // Values extracted from the loading and rendering of specific meshes
    float projectileRadius = squareSide / 2.5;
    float enemyRadius = squareSide / 2;
    float shooterRadius = squareSide * shooterBodyScaleY;

    // Enemy-Projectile collision
    for (auto i = enemies.begin(); i != enemies.end(); i++) {
        if (i->healthPoints == 0) {
            i->isDying = true;
        }
        else {
            for (auto j = projectiles.begin(); j != projectiles.end(); ) {
                if (!i->isDying && SameColor(i->type, j->type) && glm::distance(i->position, j->position) < projectileRadius + enemyRadius) {
                    i->healthPoints--;
                    j = projectiles.erase(j);
                }
                else {
                    j++;
                }
            }
        }
    }

    // Enemy-Shooter collision
    for (auto i = enemies.begin(); i != enemies.end(); i++) {
        for (int j = 0; j < 3; j++) {
            if (!i->isDying && tiles[i->line][j].isUsed && glm::distance(i->position, tiles[i->line][j].position) < shooterRadius + enemyRadius) {
                tiles[i->line][j].isDying = true;
            }
        }
    }
}

void Tema1::RenderGame(float deltaTimeSeconds)
{
    // Renders static GUI elements
    RenderGUI(deltaTimeSeconds);

    // Renders the selected shooter following the player cursor position
    if (selectedShooter) {
        RenderShooter(mouseX, window->GetResolution().y - mouseY, selectedType, 1);
    }

    // Checks if the button has been released on a valid tile, and renders the shooter there
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tiles[i][j].isUsed) {
                RenderShooter(tiles[i][j].position.x, tiles[i][j].position.y, tiles[i][j].usedType, tiles[i][j].scaleFactor);

                // Starts death animation
                if (tiles[i][j].isDying) {
                    tiles[i][j].scaleFactor -= deltaTimeSeconds;
                    if (tiles[i][j].scaleFactor < 0.1f) {
                        tiles[i][j].isUsed = false;
                        tiles[i][j].isDying = false;
                        tiles[i][j].scaleFactor = 1;
                    }
                }
                else {
                    // Determines whether the rendered shooter is being attacked by the same type of hexagon, in which case it adds a new projectile
                    if (tiles[i][j].attackedByOrange || tiles[i][j].attackedByBlue || tiles[i][j].attackedByYellow || tiles[i][j].attackedByPurple) {
                        Star newProjectile;
                        newProjectile.line = i;
                        newProjectile.position = tiles[i][j].position;
                        newProjectile.rotation = 0;
                        if (tiles[i][j].attackedByOrange && tiles[i][j].usedType == "shooterOrange") {
                            newProjectile.type = "starOrange";
                            // For each type of projectile, rendered at every 3 seconds
                            if (tiles[i][j].timer < 0) {
                                tiles[i][j].timer = 3;
                                projectiles.push_back(newProjectile);
                            }
                        }
                        else if (tiles[i][j].attackedByBlue && tiles[i][j].usedType == "shooterBlue") {
                            newProjectile.type = "starBlue";
                            if (tiles[i][j].timer < 0) {
                                tiles[i][j].timer = 3;
                                projectiles.push_back(newProjectile);
                            }
                        }
                        else if (tiles[i][j].attackedByYellow && tiles[i][j].usedType == "shooterYellow") {
                            newProjectile.type = "starYellow";
                            if (tiles[i][j].timer < 0) {
                                tiles[i][j].timer = 3;
                                projectiles.push_back(newProjectile);
                            }
                        }
                        else if (tiles[i][j].attackedByPurple && tiles[i][j].usedType == "shooterPurple") {
                            newProjectile.type = "starPurple";
                            if (tiles[i][j].timer < 0) {
                                tiles[i][j].timer = 3;
                                projectiles.push_back(newProjectile);
                            }
                        }
                    }
                }
            }
        }
    }

    RenderProjectiles(deltaTimeSeconds);

    RenderPickups(deltaTimeSeconds);

    RenderEnemies(deltaTimeSeconds);
    
    DetectCollisions();
}

void Tema1::RenderGameOver(float deltaTimeSeconds)
{
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(window->GetResolution().x / 2, window->GetResolution().y / 2 - 150, -200);
    modelMatrix *= transform3D::RotateOY(skullRotation);
    modelMatrix *= transform3D::RotateOX(RADIANS(-90));
    modelMatrix *= transform3D::Scale(10, 10, 10);
    RenderMesh(meshes["skull"], shaders["VertexNormal"], modelMatrix);
    skullRotation -= 4 * deltaTimeSeconds;
}

void Tema1::Update(float deltaTimeSeconds)
{
    if (playerLives) {
        RenderGame(deltaTimeSeconds);
    }
    else {
        RenderGameOver(deltaTimeSeconds);
    }
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Rendered shooter follows the mouse for drag and drop
    this->mouseX = mouseX;
    this->mouseY = mouseY;
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Axes allignment to detect the hitboxes of the selectable types
    bool is_orange = (borderSpacing <= mouseX && mouseX <= borderSpacing + squareSide 
                      && borderSpacing <= mouseY && mouseY <= borderSpacing + squareSide);
    bool is_blue = (borderSpacing + squareSide + shooterSpacing <= mouseX && mouseX <= borderSpacing + 2 * squareSide + shooterSpacing
                    && borderSpacing <= mouseY && mouseY <= borderSpacing + squareSide);
    bool is_yellow = (borderSpacing + 2 * squareSide + 2 * shooterSpacing <= mouseX && mouseX <= borderSpacing + 3 * squareSide + 2 * shooterSpacing
                      && borderSpacing <= mouseY && mouseY <= borderSpacing + squareSide);
    bool is_purple = (borderSpacing + 3 * squareSide + 3 * shooterSpacing <= mouseX && mouseX <= borderSpacing + 4 * squareSide + 3 * shooterSpacing
                      && borderSpacing <= mouseY && mouseY <= borderSpacing + squareSide);
    // Stops rendering pickup stars if player left-clicks on them
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (is_orange && playerStars >= 1) {
            selectedShooter = true;
            selectedType = "shooterOrange";
        }
        else if (is_blue && playerStars >= 2) {
            selectedShooter = true;
            selectedType = "shooterBlue";
        }
        else if (is_yellow && playerStars >= 3) {
            selectedShooter = true;
            selectedType = "shooterYellow";
        }
        else if (is_purple && playerStars >= 4) {
            selectedShooter = true;
            selectedType = "shooterPurple";
        }
        for (auto i = pickupPositions.begin(); i != pickupPositions.end(); i++) {
            // Axes allignment for pickup stars
            float leftBound = i->x - squareSide / 5;
            float rightBound = i->x + squareSide / 5;
            float downBound = i->y - squareSide / 5;
            float upBound = i->y + squareSide / 5;
            if (leftBound <= mouseX && mouseX <= rightBound && downBound <= window->GetResolution().y - mouseY && window->GetResolution().y - mouseY <= upBound) {
                pickupPositions.erase(i);
                playerStars = std::min(playerStars + 1, MAX_PLAYER_STARS);
                break;
            }
        }
    }

    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT)) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (IsInTile(mouseX, mouseY, i, j)) {
                    tiles[i][j].isDying = true;
                }
            }
        }
    }
}

// Checks if the mouse position (mouseX, mouseY) is in the bounds of tiles[i][j]
bool Tema1::IsInTile(int mouseX, int mouseY, int i, int j) {
    return (tiles[i][j].position.x - cx <= mouseX && mouseX <= tiles[i][j].position.x + cx
            && tiles[i][j].position.y - cy <= (window->GetResolution().y - mouseY)
            && (window->GetResolution().y - mouseY) <= tiles[i][j].position.y + cy);
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Release render for drag and drop
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (selectedShooter && IsInTile(mouseX, mouseY, i, j) && !tiles[i][j].isUsed) {
                tiles[i][j].isUsed = true;
                tiles[i][j].usedType = selectedType;
                if (selectedType == "shooterOrange") {
                    playerStars--;
                }
                else if (selectedType == "shooterBlue") {
                    playerStars -= 2;
                }
                else if (selectedType == "shooterYellow") {
                    playerStars -= 3;
                }
                else if (selectedType == "shooterPurple") {
                    playerStars -= 4;
                }
            }
        }
    }
    selectedShooter = false;
}
