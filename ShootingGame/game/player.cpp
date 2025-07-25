#include "player.h"

#include "color.h"
#include "direct3d.h"
#include "key_logger.h"
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "bullet.h"

constexpr XMFLOAT2 PLAYER_SIZE = {64.0f, 64.0f};
constexpr float PLAYER_SPEED_ACCERATION = 0.5f;

static XMFLOAT2 g_PlayerPosition{};
static XMFLOAT2 g_PlayerVelocity{};
static int g_PlayerTexId = -1;

static Circle g_PlayerCollision{{32.0f, 32.0f}, 32.0f};
static bool g_PlayerIsEnable = true;

void Player_Initialize(const XMFLOAT2& position)
{
    g_PlayerPosition = position;
    g_PlayerVelocity = {0.0f, 0.0f};
    g_PlayerIsEnable = true;

    g_PlayerTexId = Texture_Load(L"assets/ship_J.png");
}

void Player_Finalize()
{
}

void Player_Update(double elapsed_time)
{
    if (!g_PlayerIsEnable) return;

    XMVECTOR position = XMLoadFloat2(&g_PlayerPosition);
    XMVECTOR velocity = XMLoadFloat2(&g_PlayerVelocity);

    // 画面端に到達したら止まる
    XMVECTOR direction{};

    if (KeyLogger_IsPressed(KK_W))
    {
        direction += {0.0f, -1.0f};
    }
    if (KeyLogger_IsPressed(KK_S))
    {
        direction += {0.0f, 1.0f};
    }
    if (KeyLogger_IsPressed(KK_A))
    {
        direction += {-1.0f, 0.0f};
    }
    if (KeyLogger_IsPressed(KK_D))
    {
        direction += {1.0f, 0.0f};
    }

    direction = XMVector2Normalize(direction);

    velocity += direction * PLAYER_SPEED_ACCERATION;
    position += velocity;
    velocity *= 0.9f;

    XMFLOAT2 positionAfter{};

    // 画面端に到達したら止まる
    XMStoreFloat2(&positionAfter, position);
    if (positionAfter.x < 0.0f) positionAfter.x = 0.0f;
    if (positionAfter.x > Direct3D_GetBackBufferWidth() - PLAYER_SIZE.x)
        positionAfter.x = Direct3D_GetBackBufferWidth()
            - PLAYER_SIZE.x;
    if (positionAfter.y < 0.0f) positionAfter.y = 0.0f;
    if (positionAfter.y > Direct3D_GetBackBufferHeight() - PLAYER_SIZE.y)
        positionAfter.y =
            Direct3D_GetBackBufferHeight() - PLAYER_SIZE.y;

    g_PlayerPosition = positionAfter;
    XMStoreFloat2(&g_PlayerVelocity, velocity);

    /************************************************************
    // physics simulation, used for any fps
    velocity += direction * 6000000.0f / 2500.0f * elapsed_time;
    position += velocity * elapsed_time;
    velocity += -velocity * 4.0f * elapsed_time;
    ************************************************************/

    // 弾の発射
    if (KeyLogger_IsTrigger(KK_SPACE))
    {
        Bullet_Create({
            g_PlayerPosition.x + PLAYER_SIZE.x * 0.5f, g_PlayerPosition.y + (PLAYER_SIZE.y - 64.0f) * 0.5f
        });
    }
}

void Player_Draw()
{
    if (!g_PlayerIsEnable) return;

    Sprite_Draw(g_PlayerTexId,
                g_PlayerPosition.x, g_PlayerPosition.y,
                PLAYER_SIZE.x, PLAYER_SIZE.y,
                90 * (XM_PI / 180.0f), Color::NAVY
    );
}

bool Player_IsEnable()
{
    return g_PlayerIsEnable;
}

Circle Player_GetCollision()
{
    float cx = g_PlayerCollision.center.x + g_PlayerPosition.x;
    float cy = g_PlayerCollision.center.y + g_PlayerPosition.y;

    return {{cx, cy}, g_PlayerCollision.radius};
}

void Player_Destroy()
{
    g_PlayerIsEnable = false;
}
