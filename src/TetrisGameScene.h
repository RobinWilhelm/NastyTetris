#pragma once

#include "SimpleMath.h"
namespace DXSM = DirectX::SimpleMath;

#include "Scene.h"
#include "Tetromino.h"
#include "Sprite.h"
#include "AssetManager.h"

#include <memory>
#include <cstdint>
#include <random>

struct FieldElement
{
    bool        Active = 0;
    DXSM::Color Color  = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct RemovedElement
{
    DXSM::Color   Color = { 0.0f, 0.0f, 0.0f, 0.0f };
    DXSM::Vector2 Position;
    DXSM::Vector2 PositionNext;
    DXSM::Vector2 Velocity;
};

class TetrisGameScene : public Scene
{
public:
    TetrisGameScene();
    virtual ~TetrisGameScene();

    // Geerbt über Scene
    void fixed_update( double deltaTime ) override;
    void interpolate_and_create_rendercommands( float factor, GPURenderer* pRenderer ) override;
    bool handle_event( SDL_Event* pEvent ) override;

    void create_playingfield( uint16_t width = 10, uint16_t height = 20, uint16_t spawnAreaHeight = 4 );
    void destroy_playingfield();

    int get_field_width() const;     // Width of GameField in elements
    int get_field_height() const;    // Height of GameField + Spawnarea in elements

    int get_total_width() const;     // Width of GameField + Borders in elements
    int get_total_height() const;    // Height of GameField + Spawnarea + Borders in elements

    float get_element_x_coord( int elem_x );
    float get_element_y_coord( int elem_y );

    bool check_collision( Direction dir ) const;

private:
    FieldElement* m_gameField       = nullptr;    // represents the playing field with all static pieces (not the currently active one)
    int           m_fieldWidth      = 0;
    int           m_fieldHeight     = 0;
    int           m_spawnAreaHeight = 0;
    int           m_borderThickness = 1;

    std::mt19937                                                         m_rngEngine;
    std::uniform_int_distribution<std::underlying_type_t<TetrominoType>> m_tetrominoDistribution;

    std::unique_ptr<Tetromino> m_activeTetromino         = nullptr;
    double                     m_nextTetrominoActionTime = 1.0;

    bool m_keyDown_A = false;
    bool m_keyDown_S = false;
    bool m_keyDown_D = false;
    bool m_keyDown_R = false;

    AssetView<Sprite> m_tileSprite;

    DXSM::Vector2               m_gravityAccel = { 0.0f, 600.0f };
    std::vector<RemovedElement> m_removedElements;
};
