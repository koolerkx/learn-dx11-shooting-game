#include "game.h"

#include "Audio.h"
#include "bullet.h"
#include "direct3d.h"
#include "enemy_spawner.h"
#include "player.h"
#include "effect.h"
#include "fade.h"
#include "key_logger.h"
#include "score.h"
#include "texture.h"

void hitJudgementBilletVSEnemy();
void hitJudgementPlayerVSEnemy();

int g_bgmId = -1;
bool g_isGameStart = false;

void Game_Initialize()
{
    Player_Initialize({64.0f, 64.0f});
    Bullet_Initialize();
    EnemySpawner_Initialize();

    EnemySpawner_Create({
                            static_cast<float>(Direct3D_GetBackBufferWidth()) - 100.0f,
                            static_cast<float>(Direct3D_GetBackBufferHeight()) / 2.0f
                        }, EnemyTypeID::GREEN,
                        3.0f, 0.5, 5);


    EnemySpawner_Create({
                            static_cast<float>(Direct3D_GetBackBufferWidth()) - 100.0f,
                            static_cast<float>(Direct3D_GetBackBufferHeight()) / 2.0f
                        }, EnemyTypeID::RED,
                        4.0f, 0.5, 5);
    Effect_Initialize();
    Score_Initialize(100.0f, 100.0f, 3);

    g_bgmId = LoadAudio("assets/audio/gunka.wav");

    Fade_Start(1.0, false);
}

void Game_Finalize()
{
    Player_Finalize();
    Bullet_Finalize();
    EnemySpawner_Finalize();
    Effect_Finalize();
    Score_Finalize();

    UnloadAudio(g_bgmId);
    // Texture_AllRelease();
}

void Game_Update(double elapsed_time)
{
    if (!g_isGameStart && Fade_GetState() == FADE_STATE::FINISHED_IN)
    {
        // PlayAudio(g_bgmId, true);
        g_isGameStart = true;
    }

    if (KeyLogger_IsTrigger(KK_F))
    {
        Fade_Start(1.0, true, Color::WHITE);
    }

    EnemySpawner_Update(elapsed_time);
    Player_Update(elapsed_time);
    Bullet_Update(elapsed_time);
    Enemy_Update(elapsed_time);

    hitJudgementBilletVSEnemy();
    hitJudgementPlayerVSEnemy();

    Effect_Update(elapsed_time);
    Score_Update();
}

void Game_Draw()
{
    Enemy_Draw();
    Bullet_Draw();
    Player_Draw();

    Effect_Draw();
    Score_Draw();
}

void hitJudgementBilletVSEnemy()
{
    for (int bi = 0; bi < BULLET_MAX; bi++)
    {
        for (int ei = 0; ei < ENEMY_MAX; ei++)
        {
            if (!Bullet_IsEnable(bi)) continue;
            if (!Enemy_IsEnable(ei)) continue;

            if (Collision_IsOverlapCircle(Bullet_GetCollision(bi), Enemy_GetCollision(ei)))
            {
                Bullet_Destroy(bi);
                Enemy_Damage(ei);
                Score_AddScore(10);
            }
        }
    }
}

void hitJudgementPlayerVSEnemy()
{
    for (int ei = 0; ei < ENEMY_MAX; ei++)
    {
        if (Player_IsEnable())
        {
            if (Collision_IsOverlapCircle(Player_GetCollision(), Enemy_GetCollision(ei)))
            {
                Player_Destroy();
                Enemy_Destroy(ei);
            }
        }
    }
}
