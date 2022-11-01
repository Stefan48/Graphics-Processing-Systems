#include "lab_m2/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <time.h>

#include "stb/stb_image.h"

using namespace std;
using namespace m2;


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


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 2.5, 4), glm::quat(glm::vec3(-55 * TO_RADIANS, 0, 0)));
    camera->Update();

    std::string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube");
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders");

    // Meshes
    {
        Mesh* mesh = new Mesh("cube");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        // Create a line mesh
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0, 0)),
            VertexFormat(glm::vec3(0, 1, 0))
        };

        vector<unsigned int> indices =
        {
            0, 1
        };

        meshes["line"] = new Mesh("line");
        meshes["line"]->InitFromData(vertices, indices);
        meshes["line"]->SetDrawMode(GL_LINES);
    }

    // Shaders
    {
        Shader *shader = new Shader("ShaderNormal");
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader *shader = new Shader("ShaderBoard");
        shader->AddShader(PATH_JOIN(shaderPath, "Board.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Board.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader *shader = new Shader("ShaderPawns");
        shader->AddShader(PATH_JOIN(shaderPath, "Pawns.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Pawns.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Pawns.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader *shader = new Shader("ShaderParticles");
        shader->AddShader(PATH_JOIN(shaderPath, "Particle.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Particle.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Particle.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Cubemap
    cubeMapTextureID = UploadCubeMapTexture(
        PATH_JOIN(texturePath, "pos_x.png"),
        PATH_JOIN(texturePath, "pos_y.png"),
        PATH_JOIN(texturePath, "pos_z.png"),
        PATH_JOIN(texturePath, "neg_x.png"),
        PATH_JOIN(texturePath, "neg_y.png"),
        PATH_JOIN(texturePath, "neg_z.png"));

    numCurveInstances = 10;
    // number of generated points for each segment of the pawn
    numGeneratedPoints = 5;
    // Define control points
    // white pawns
    control_p0[0][0] = glm::vec3(-5.5, 0, 1.0);
    control_p1[0][0] = glm::vec3(-3.5, 3.0, 1.0);
    control_p2[0][0] = glm::vec3(-5.5, 3.5, 1.0);
    control_p3[0][0] = glm::vec3(-1.5, 5.0, 1.0);

    control_p0[0][1] = glm::vec3(-1.5, 5.0, 1.0);
    control_p1[0][1] = glm::vec3(-2.5, 7.5, 1.0);
    control_p2[0][1] = glm::vec3(-1.5, 8, 1.0);
    control_p3[0][1] = glm::vec3(-1.9, 9.5, 1.0);

    control_p0[0][2] = glm::vec3(-1.9, 9.5, 1.0);
    control_p1[0][2] = glm::vec3(0.75, 12.5, 1.0);
    control_p2[0][2] = glm::vec3(4.5, 14.0, 1.0);
    control_p3[0][2] = glm::vec3(0.0, 16.5, 0.0);

    // black pawns
    control_p0[1][0] = glm::vec3(-5.25, 0, 1.0);
    control_p1[1][0] = glm::vec3(-1.5, 3.0, 1.0);
    control_p2[1][0] = glm::vec3(-0.75, 3.5, 1.0);
    control_p3[1][0] = glm::vec3(-2.25, 5.0, 1.0);

    control_p0[1][1] = glm::vec3(-2.25, 5.0, 1.0);
    control_p1[1][1] = glm::vec3(-5, 7.0, 1.0);
    control_p2[1][1] = glm::vec3(-3.5, 10.5, 1.0);
    control_p3[1][1] = glm::vec3(-2.25, 12, 1.0);

    control_p0[1][2] = glm::vec3(-2.25, 12, 1.0);
    control_p1[1][2] = glm::vec3(-3.75, 15, 1.0);
    control_p2[1][2] = glm::vec3(-6.75, 15.5, 1.0);
    control_p3[1][2] = glm::vec3(0.0, 17, 0.0);

    // Pawns
    pawnScale = 0.025f;
    pawns[0][0] = Pawn(0, 0, 0, true);
    pawns[0][1] = Pawn(1, 0, 2, true);
    pawns[0][2] = Pawn(2, 0, 4, true);
    pawns[0][3] = Pawn(3, 0, 6, true);
    pawns[0][4] = Pawn(4, 1, 1, true);
    pawns[0][5] = Pawn(5, 1, 3, true);
    pawns[0][6] = Pawn(6, 1, 5, true);
    pawns[0][7] = Pawn(7, 1, 7, true);
    pawns[0][8] = Pawn(8, 2, 0, true);
    pawns[0][9] = Pawn(9, 2, 2, true);
    pawns[0][10] = Pawn(10, 2, 4, true);
    pawns[0][11] = Pawn(11, 2, 6, true);

    pawns[1][0] = Pawn(12, 5, 1, false);
    pawns[1][1] = Pawn(13, 5, 3, false);
    pawns[1][2] = Pawn(14, 5, 5, false);
    pawns[1][3] = Pawn(15, 5, 7, false);
    pawns[1][4] = Pawn(16, 6, 0, false);
    pawns[1][5] = Pawn(17, 6, 2, false);
    pawns[1][6] = Pawn(18, 6, 4, false);
    pawns[1][7] = Pawn(19, 6, 6, false);
    pawns[1][8] = Pawn(20, 7, 1, false);
    pawns[1][9] = Pawn(21, 7, 3, false);
    pawns[1][10] = Pawn(22, 7, 5, false);
    pawns[1][11] = Pawn(23, 7, 7, false);

    // Board
    squareLength = 0.45f;
    board[0][0] = Square(24, -1.6f, 3.25f, false);
    for (int j = 1; j < 8; ++j)
    {
        board[0][j] = Square(24 + j, board[0][j-1].x + squareLength, board[0][j-1].z, !board[0][j-1].isWhite);
    }
    for (int i = 1; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            board[i][j] = Square(24 + i * 8 + j, board[i - 1][j].x, board[i - 1][j].z - squareLength, !board[i - 1][j].isWhite);
        }
    }
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 12; ++j)
        {
            board[pawns[i][j].line][pawns[i][j].column].pawn = &pawns[i][j];
        }
    }

    // initialize particle system
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle4.png");
    particleEffect = new ParticleEffect<Particle>();
    particleEffect->Generate(numParticles, true);

    for (unsigned int i = 0; i < numParticles; ++i)
    {
        particleSpeeds[i] = glm::vec4(sin(i) / 10.0f, 1.0f, cos(i) / 10.0f, 1.0f);
    }
}


void Tema1::FrameStart()
{
}

void Tema1::RenderMeshInstanced(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, int instances, const glm::vec3 &color)
{
    if (!mesh || !shader || !shader->GetProgramID())
    {
        return;
    }
        
    // Render an object using the specified shader
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object instanced
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, instances);
}


void Tema1::Update(float deltaTimeSeconds)
{
    ClearScreen();
    auto camera = GetSceneCamera();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the cubemap
    {
        Shader *shader = shaders["ShaderNormal"];
        shader->Use();

        glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));

        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        glUniform1i(shader->GetUniformLocation("no_color"), noColor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        int loc_texture = shader->GetUniformLocation("texture_cubemap");
        glUniform1i(loc_texture, 0);

        meshes["cube"]->Render();
    }

    // Draw the board
    {
        Shader *shader = shaders["ShaderBoard"];
        shader->Use();

        // Send to shader info that is common for all the squares
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        glUniform1i(shader->GetUniformLocation("no_color"), noColor);

        auto cameraPosition = camera->m_transform->GetWorldPosition();
        glUniform3f(shader->GetUniformLocation("camera_position"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
        
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                glm::mat4 modelMatrix = glm::mat4(1);                
                modelMatrix = glm::translate(modelMatrix, glm::vec3(board[i][j].x, 0, board[i][j].z));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(squareLength/2.0f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                glm::vec4 color = glm::vec4(board[i][j].isHighlighted ? green : (board[i][j].isWhite ? white : black), ((float)board[i][j].id) / 255.0f);
                glUniform4fv(shader->GetUniformLocation("color"), 1, glm::value_ptr(color));

                meshes["quad"]->Render();
            }
        }
    }

    // Draw the pawns
    {
        Shader *shader = shaders["ShaderPawns"];
        shader->Use();

        // Send to shader info that is common for all the pawns
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        glUniform1i(shader->GetUniformLocation("no_color"), noColor);

        auto cameraPosition = camera->m_transform->GetWorldPosition();
        glUniform3f(shader->GetUniformLocation("camera_position"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);

        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), numCurveInstances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), numGeneratedPoints);

        // update (and draw) moving pawn
        UpdateMovingPawn(deltaTimeSeconds);
        if (movingPawn != NULL)
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, currentPos);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(pawnScale));

            glm::vec4 color = glm::vec4(movingPawn->isWhite ? white : black, ((float)movingPawn->id) / 255.0f);
            glUniform4fv(shader->GetUniformLocation("color"), 1, glm::value_ptr(color));

            int side = movingPawn->isWhite ? 0 : 1;
            glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 3, glm::value_ptr(control_p0[side][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 3, glm::value_ptr(control_p1[side][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 3, glm::value_ptr(control_p2[side][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 3, glm::value_ptr(control_p3[side][0]));

            RenderMeshInstanced(meshes["line"], shader, modelMatrix, numCurveInstances);
        }

        for (int i = 0; i < 2; ++i)
        {
            glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 3, glm::value_ptr(control_p0[i][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 3, glm::value_ptr(control_p1[i][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 3, glm::value_ptr(control_p2[i][0]));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 3, glm::value_ptr(control_p3[i][0]));

            for (int j = 0; j < 12; ++j)
            {
                if (pawns[i][j].isActive && &pawns[i][j] != movingPawn)
                {
                    int line = pawns[i][j].line, column = pawns[i][j].column;
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(board[line][column].x, 0.0f, board[line][column].z));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(pawnScale));

                    glm::vec4 color = glm::vec4(pawns[i][j].isHighlighted ? green : (pawns[i][j].isWhite ? white : black), ((float)pawns[i][j].id) / 255.0f);
                    glUniform4fv(shader->GetUniformLocation("color"), 1, glm::value_ptr(color));

                    RenderMeshInstanced(meshes["line"], shader, modelMatrix, numCurveInstances);
                }
            }
        }
    }

    // Draw the particles
    {
        if (particlesActive)
        {
            Shader *shader = shaders["ShaderParticles"];
            if (shader->GetProgramID())
            {
                shader->Use();
                TextureManager::GetTexture("particle4.png")->BindToTextureUnit(GL_TEXTURE0);
                glUniform1i(shader->GetUniformLocation("no_color"), noColor);
                particleEffect->Render(GetSceneCamera(), shader);
            }
            particlesTimer += particlesTimerStep * deltaTimeSeconds;
            if (particlesTimer >= particlesTimerLimit)
            {
                particlesActive = false;
            }
        }
    }
}


void Tema1::FrameEnd()
{
}


unsigned int Tema1::UploadCubeMapTexture(const std::string &pos_x, const std::string &pos_y, const std::string &pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z)
{
    int width, height, chn;

    unsigned char* data_pos_x = stbi_load(pos_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_y = stbi_load(pos_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_z = stbi_load(pos_z.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_x = stbi_load(neg_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_y = stbi_load(neg_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_z = stbi_load(neg_z.c_str(), &width, &height, &chn, 0);

    unsigned int textureID = 0;
    // Create the texture
    glGenTextures(1, &textureID);

    // Bind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxAnisotropy;

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load texture information for each face
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_z);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_z);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Free memory
    SAFE_FREE(data_pos_x);
    SAFE_FREE(data_pos_y);
    SAFE_FREE(data_pos_z);
    SAFE_FREE(data_neg_x);
    SAFE_FREE(data_neg_y);
    SAFE_FREE(data_neg_z);

    return textureID;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        noColor = !noColor;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    // left click is GLFW_MOUSE_BUTTON_2, right click is GLFW_MOUSE_BUTTON_3
    if (button == GLFW_MOUSE_BUTTON_2)
    {
        unsigned char data[4];
        glReadPixels(mouseX, window->props.resolution.y - mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int id = data[3];
        
        if (!noColor && movingPawn == NULL)
        {
            if (id < 24)
            {
                // clicked on pawn
                Pawn *selectedPawn = &pawns[id / 12][id % 12];
                if (selectedPawn->isWhite != isWhiteTurn)
                {
                    // deselect highlighted pawn (if any)
                    if (highlightedPawn != NULL)
                    {
                        highlightedPawn->isHighlighted = false;
                        UnhighlightPossibleMoves(highlightedPawn);
                        highlightedPawn = NULL;
                    }
                }
                else if (selectedPawn != highlightedPawn)
                {
                    if (highlightedPawn != NULL)
                    {
                        highlightedPawn->isHighlighted = false;
                        UnhighlightPossibleMoves(highlightedPawn);
                    }
                    highlightedPawn = selectedPawn;
                    selectedPawn->isHighlighted = true;
                    HighlightPossibleMoves(selectedPawn);
                }
            }
            else if (id < 88)
            {
                // clicked on square
                int l = (id - 24) / 8, c = (id - 24) % 8;
                if (!board[l][c].isHighlighted)
                {
                    // deselect highlighted pawn (if any)
                    if (highlightedPawn != NULL)
                    {
                        highlightedPawn->isHighlighted = false;
                        UnhighlightPossibleMoves(highlightedPawn);
                        highlightedPawn = NULL;
                    }
                }
                else
                {
                    // apply move
                    isWhiteTurn = !isWhiteTurn;
                    movingPawn = highlightedPawn;
                    highlightedPawn->isHighlighted = false;
                    UnhighlightPossibleMoves(highlightedPawn);
                    highlightedPawn = NULL;
                    float currentX = board[movingPawn->line][movingPawn->column].x;
                    float currentZ = board[movingPawn->line][movingPawn->column].z;
                    if (abs(movingPawn->line - l) == 2)
                    {
                        // capture a pawn
                        int capturedL = (movingPawn->line + l) / 2;
                        int capturedC = (movingPawn->column + c) / 2;
                        board[capturedL][capturedC].pawn->isActive = false;
                        board[capturedL][capturedC].pawn = NULL;
                        particlesActive = true;
                        particlesTimer = 0.0f;
                        ResetParticleSystem(glm::vec3(board[capturedL][capturedC].x, 0, board[capturedL][capturedC].z));
                    }
                    currentPos = glm::vec3(currentX, 0.0f, currentZ);
                    destPos = glm::vec3(board[l][c].x, 0.0f, board[l][c].z);
                    jumpDistance = glm::distance(currentPos, destPos);
                    board[movingPawn->line][movingPawn->column].pawn = NULL;
                    movingPawn->line = l;
                    movingPawn->column = c;
                    board[l][c].pawn = movingPawn;
                }
            }
            else
            {
                // clicked outisde the board
                // deselect highlighted pawn (if any)
                if (highlightedPawn != NULL)
                {
                    highlightedPawn->isHighlighted = false;
                    UnhighlightPossibleMoves(highlightedPawn);
                    highlightedPawn = NULL;
                }
            }
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}

vector<pair<int, int>> Tema1::GetPossibleMoves(Pawn *pawn)
{
    vector<pair<int, int>> possibleMoves;
    int l = pawn->line, c = pawn->column;
    if (pawn->isWhite)
    {
        if (l < 7)
        {
            if (c > 0 && board[l + 1][c - 1].pawn == NULL)
            {
                possibleMoves.push_back({ l + 1, c - 1 });
            }
            if (c < 7 && board[l + 1][c + 1].pawn == NULL)
            {
                possibleMoves.push_back({ l + 1, c + 1 });
            }
            if (l < 6)
            {
                if (c > 1 && board[l + 1][c - 1].pawn != NULL && board[l + 1][c - 1].pawn->isWhite == !pawn->isWhite && board[l + 2][c - 2].pawn == NULL)
                {
                    possibleMoves.push_back({ l + 2, c - 2 });
                }
                if (c < 6 && board[l + 1][c + 1].pawn != NULL && board[l + 1][c + 1].pawn->isWhite == !pawn->isWhite && board[l + 2][c + 2].pawn == NULL)
                {
                    possibleMoves.push_back({ l + 2, c + 2 });
                }
            }
        }
    }
    else
    {
        if (l > 0)
        {
            if (c > 0 && board[l - 1][c - 1].pawn == NULL)
            {
                possibleMoves.push_back({ l - 1, c - 1 });
            }
            if (c < 7 && board[l - 1][c + 1].pawn == NULL)
            {
                possibleMoves.push_back({ l - 1, c + 1 });
            }
            if (l > 1)
            {
                if (c > 1 && board[l - 1][c - 1].pawn != NULL && board[l - 1][c - 1].pawn->isWhite == !pawn->isWhite && board[l - 2][c - 2].pawn == NULL)
                {
                    possibleMoves.push_back({ l - 2, c - 2 });
                }
                if (c < 6 && board[l - 1][c + 1].pawn != NULL && board[l - 1][c + 1].pawn->isWhite == !pawn->isWhite && board[l - 2][c + 2].pawn == NULL)
                {
                    possibleMoves.push_back({ l - 2, c + 2 });
                }
            }
        }
    }
    return possibleMoves;
}

void Tema1::HighlightPossibleMoves(Pawn *pawn)
{
    vector<pair<int, int>> possibleMoves = GetPossibleMoves(pawn);
    for (auto it = possibleMoves.begin(); it != possibleMoves.end(); ++it)
    {
        board[it->first][it->second].isHighlighted = true;
    }
}

void Tema1::UnhighlightPossibleMoves(Pawn *pawn)
{
    vector<pair<int, int>> possibleMoves = GetPossibleMoves(pawn);
    for (auto it = possibleMoves.begin(); it != possibleMoves.end(); ++it)
    {
        board[it->first][it->second].isHighlighted = false;
    }
}

void Tema1::UpdateMovingPawn(float deltaTime)
{
    if (movingPawn != NULL)
    {
        float dist = glm::distance(glm::vec3(currentPos.x, 0, currentPos.z), destPos);
        if (dist <= jumpDistance / 2.0f)
        {
            // pawn goes down
            currentPos.y -= jumpStepVertical * deltaTime;
        }
        else
        {
            // pawn goes up
            currentPos.y += jumpStepVertical * deltaTime;
        }
        if (currentPos.x > destPos.x)
        {
            currentPos.x -= jumpStepHorizontal * deltaTime;
            if (currentPos.x <= destPos.x)
            {
                movingPawn = NULL;
                return;
            }
        }
        else
        {
            currentPos.x += jumpStepHorizontal * deltaTime;
            if (currentPos.x >= destPos.x)
            {
                movingPawn = NULL;
                return;
            }
        }
        if (currentPos.z > destPos.z)
        {
            currentPos.z -= jumpStepHorizontal * deltaTime;
            if (currentPos.z <= destPos.z)
            {
                movingPawn = NULL;
                return;
            }
        }
        else
        {
            currentPos.z += jumpStepHorizontal * deltaTime;
            if (currentPos.z >= destPos.z)
            {
                movingPawn = NULL;
                return;
            }
        }
    }
}

void Tema1::ResetParticleSystem(glm::vec3 capturePosition)
{
    auto particleSSBO = particleEffect->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    for (unsigned int i = 0; i < numParticles; i++)
    {
        glm::vec4 pos(1);
        pos.x = capturePosition.x;
        pos.y = (rand() % 500) / 1000.0f;
        pos.z = capturePosition.z;
        data[i].SetParams(pos, particleSpeeds[i]);
    }
    particleSSBO->SetBufferData(data);
}
