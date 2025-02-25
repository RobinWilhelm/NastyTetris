#include "iepch.h"
#include "TetrisGameScene.h"

#include "CoreAPI.h"

#include "Application.h"
#include "Renderer.h"
#include "Sprite2DPipeline.h"
#include "AssetManager.h"

TetrisGameScene::TetrisGameScene()
{
    std::random_device rd;
    m_rngEngine.seed( rd() );
    auto param = m_tetrominoDistribution.param();
    param._Init( static_cast<std::underlying_type_t<TetrominoType>>( TetrominoType::O ), static_cast<std::underlying_type_t<TetrominoType>>( TetrominoType::J ) );
    m_tetrominoDistribution.param( param );

    auto tileSpriteOpt = CoreAPI::get_assetmanager()->require_asset<Sprite>( "tile.png" );
    m_tileSprite       = tileSpriteOpt.value();
    m_tileSprite.get()->create_device_ressources( CoreAPI::get_gpurenderer() );

    create_playingfield( 10, 20 );
}

TetrisGameScene::~TetrisGameScene()
{
    destroy_playingfield();
}

void TetrisGameScene::create_playingfield( uint16_t width, uint16_t height, uint16_t spawnAreaHeight )
{
    m_gameField       = new FieldElement[width * ( height + spawnAreaHeight /* spawnarea for the active pieces */ )];
    m_spawnAreaHeight = spawnAreaHeight;
    m_fieldWidth      = width;
    m_fieldHeight     = height;
}

void TetrisGameScene::destroy_playingfield()
{
    delete[] m_gameField;
}

uint16_t TetrisGameScene::get_playingfield_width_in_elements() const
{
    return m_fieldWidth;
}

uint16_t TetrisGameScene::get_playingfield_height_in_elements() const
{
    return m_fieldHeight + m_spawnAreaHeight;
}

uint16_t TetrisGameScene::get_total_width_in_elements() const
{
    return m_fieldWidth + 2 * m_borderThickness;
}

uint16_t TetrisGameScene::get_total_height_in_elements() const
{
    return m_fieldHeight + m_spawnAreaHeight + 2 * m_borderThickness;
}

bool TetrisGameScene::check_collision( Direction dir ) const
{
    for ( const Element& elem : m_activeTetromino->get_structure().Elements ) {
        switch ( dir ) {
        case Direction::Down:
        {
            if ( elem.y + 1 >= get_playingfield_height_in_elements() ||    // check for collision with south border
                 ( elem.y + 1 < get_playingfield_height_in_elements() &&
                   m_gameField[( elem.y + 1 ) * get_playingfield_width_in_elements() + elem.x].Active ) )    // check for collision with field element directly underneath
            {
                return true;
            }
            break;
        }
        case Direction::Left:
        {
            if ( elem.x - 1 < 0 ||                                                                                         // check for collision with left border
                 ( elem.x - 1 > 0 && m_gameField[elem.y * get_playingfield_width_in_elements() + elem.x - 1].Active ) )    // check for collision with field element on the left
            {
                return true;
            }
            break;
        }
        case Direction::Right:
        {
            if ( elem.x + 1 >= get_playingfield_width_in_elements() ||    // check for collision with left border
                 ( elem.x + 1 < get_playingfield_width_in_elements() &&
                   m_gameField[elem.y * get_playingfield_width_in_elements() + elem.x + 1].Active ) )    // check for collision with field element on the left
            {
                return true;
            }
            break;
        }
        case Direction::Up: break;
        }
    }
    return false;
}

void TetrisGameScene::fixed_update( double deltaTime )
{
    // handle player input
    if ( m_activeTetromino != nullptr ) {
        if ( m_keyDown_R ) {
            m_activeTetromino->rotate_once();
            m_keyDown_R = false;
        }

        if ( m_keyDown_A && !check_collision( Direction::Left ) ) {
            m_activeTetromino->move_one( Direction::Left );
            m_keyDown_A = false;
        }

        if ( m_keyDown_D && !check_collision( Direction::Right ) ) {
            m_activeTetromino->move_one( Direction::Right );
            m_keyDown_D = false;
        }

        if ( m_keyDown_S && !check_collision( Direction::Down ) ) {
            m_activeTetromino->move_one( Direction::Down );
            m_keyDown_S = false;
        }
    }

    m_nextTetrominoActionTime -= deltaTime;
    if ( m_nextTetrominoActionTime <= 0.0 ) {
        if ( m_activeTetromino == nullptr ) {
            // TODO create new random
            TetrominoType newType = static_cast<TetrominoType>( m_tetrominoDistribution( m_rngEngine ) );
            m_activeTetromino     = std::make_unique<Tetromino>( newType, Orientation::Up, static_cast<uint16_t>( get_playingfield_width_in_elements() / 2 - 1 ), static_cast<uint16_t>( 1 ) );
        }
        else {
            // fuse tetromino to the playing field if we collided
            if ( check_collision( Direction::Down ) ) {
                for ( const Element& elem : m_activeTetromino->get_structure().Elements ) {
                    FieldElement& fieldElem = m_gameField[( elem.y * get_playingfield_width_in_elements() ) + elem.x];
                    fieldElem.Active        = true;
                    fieldElem.Color         = Tetromino::get_color( m_activeTetromino->get_type() );

                    // check if we lost by checking if the tetromino got partially fused into the spawnarea
                    if ( elem.y < m_spawnAreaHeight ) {
                        // TODO: show some proper end game screen
                        CoreAPI::get_application()->shutdown();
                    }
                }

                // delete the active tetromino after collision so a new one can be created
                m_activeTetromino.reset();

                // check here if we completed a row
                for ( int y = m_spawnAreaHeight; y < get_playingfield_height_in_elements(); ++y ) {
                    bool completedRow = true;
                    for ( int x = 0; x < m_fieldWidth; ++x ) {
                        if ( m_gameField[( y * get_playingfield_width_in_elements() ) + x].Active == false ) {
                            completedRow = false;
                            break;
                        }
                    }
                    if ( completedRow ) {
                        // loop over row again and deactivate it
                        for ( int x = 0; x < m_fieldWidth; ++x ) {
                            m_gameField[( y * get_playingfield_width_in_elements() ) + x].Active = false;
                        }

                        // move all rows above down by one
                        for ( int abov = m_spawnAreaHeight; y < get_playingfield_height_in_elements(); ++y ) { }

                        // TODO: player should got some points here
                    }
                }
            }
            else {
                m_activeTetromino->move_one( Direction::Down );
            }
        }
        m_nextTetrominoActionTime = 0.5f;
    }
}

void TetrisGameScene::interpolate_and_create_rendercommands( float factor, GPURenderer* pRenderer )
{
    // actually no interpolation right now as default Tetris has no smooth movements
    (void)factor;
    (void)pRenderer;

    const std::shared_ptr<Sprite> sprite = m_tileSprite.get();

    // render playing field borders
    DXSM::Color borderColorModifier { 0.8f, 0.8f, 0.8f, 1.0f };
    for ( int x = 1; x < get_total_width_in_elements() - 1; x++ ) {
        sprite->render( static_cast<float>( x * m_tileSprite.get()->get_width() ), 0.0f, borderColorModifier );
        sprite->render( static_cast<float>( x * m_tileSprite.get()->get_width() ), static_cast<float>( ( get_total_height_in_elements() - 1 ) * m_tileSprite.get()->get_height() ),
                        borderColorModifier );
    }

    for ( int y = 0; y < get_total_height_in_elements(); y++ ) {
        sprite->render( 0.0f, static_cast<float>( y * m_tileSprite.get()->get_height() ), borderColorModifier );
        sprite->render( static_cast<float>( ( get_total_width_in_elements() - 1 ) * m_tileSprite.get()->get_width() ), static_cast<float>( y * m_tileSprite.get()->get_height() ),
                        borderColorModifier );
    }

    // render static elements
    for ( int y = 0; y < get_playingfield_height_in_elements(); y++ ) {
        for ( int x = 0; x < get_playingfield_width_in_elements(); x++ ) {
            if ( m_gameField[y * get_playingfield_width_in_elements() + x].Active ) {
                sprite->render( static_cast<float>( ( x + m_borderThickness ) * m_tileSprite.get()->get_width() ), static_cast<float>( ( y + m_borderThickness ) * m_tileSprite.get()->get_height() ),
                                m_gameField[y * get_playingfield_width_in_elements() + x].Color );
            }
        }
    }

    // render dynamic elements
    if ( m_activeTetromino ) {
        for ( const Element& elem : m_activeTetromino->get_structure().Elements ) {
            sprite->render( static_cast<float>( ( elem.x + m_borderThickness ) * m_tileSprite.get()->get_width() ),
                            static_cast<float>( ( elem.y + m_borderThickness ) * m_tileSprite.get()->get_height() ), Tetromino::get_color( m_activeTetromino->get_type() ) );
        }
    }
}

bool TetrisGameScene::handle_event( SDL_Event* pEvent )
{
    switch ( pEvent->type ) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
    {
        if ( pEvent->key.scancode == SDL_SCANCODE_A )
            m_keyDown_A = pEvent->key.down;
        if ( pEvent->key.scancode == SDL_SCANCODE_S )
            m_keyDown_S = pEvent->key.down;
        if ( pEvent->key.scancode == SDL_SCANCODE_D )
            m_keyDown_D = pEvent->key.down;
        if ( pEvent->key.scancode == SDL_SCANCODE_R )
            m_keyDown_R = pEvent->key.down;
        break;
    }
    }
    return true;
}
