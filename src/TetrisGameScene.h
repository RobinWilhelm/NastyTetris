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

class TetrisGameScene : public Scene
{
public:
    TetrisGameScene();
    virtual ~TetrisGameScene() = default;

    // Geerbt über Scene
    void fixed_update( double deltaTime ) override;
    void interpolate_and_create_rendercommands( float factor, GPURenderer* pRenderer ) override;
    bool handle_event( SDL_Event* pEvent ) override;

    void create_playingfield( uint16_t width = 10, uint16_t height = 20, uint16_t spawnAreaHeight = 4 );

    uint16_t get_playingfield_width_in_elements() const;     // Width of GameField
    uint16_t get_playingfield_height_in_elements() const;    // Height of GameField + Spawnarea

    uint16_t get_total_width_in_elements() const;     // Width of GameField + Borders
    uint16_t get_total_height_in_elements() const;    // Height of GameField + Spawnarea + Borders

    bool check_collision( Direction dir ) const;

private:
    FieldElement* m_gameField  = nullptr;    // represents the playing field with all static pieces (not the currently active one)
    uint16_t      m_fieldWidth = 0, m_fieldHeight = 0;
    uint16_t      m_spawnAreaHeight = 0;
    uint16_t      m_borderThickness = 1;

    std::mt19937                                                         m_rngEngine;
    std::uniform_int_distribution<std::underlying_type_t<TetrominoType>> m_tetrominoDistribution;

    std::unique_ptr<Tetromino> m_activeTetromino         = nullptr;
    double                     m_nextTetrominoActionTime = 1.0;

    bool m_keyDown_A = false;
    bool m_keyDown_S = false;
    bool m_keyDown_D = false;

    AssetView<Sprite> m_tileSprite;
};
