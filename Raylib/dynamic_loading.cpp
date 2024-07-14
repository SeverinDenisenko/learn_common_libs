// Copyright 2024 Severin Denisenko

#include <cstdint>

#include <boost/container/list.hpp>
#include <boost/unordered/unordered_set.hpp>

#include <raylib.h>
#include <raymath.h>

#include "../Boost/lru_map.hpp"

using integer = int32_t;
struct chunk_position_t {
    integer x;
    integer y;

    struct is_equal {
        bool operator()(const chunk_position_t& a, const chunk_position_t& b) const noexcept
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    struct hash {
        size_t operator()(const chunk_position_t& a) const noexcept
        {
            boost::hash<integer> hasher;
            return hasher(a.x * a.y);
        }
    };
};

class chunk_t {
public:
    static constexpr float scale_ { 0.1f };
    static constexpr Vector2 size_ { 64, 64 };

    chunk_t(chunk_position_t position)
        : position_(position)
    {
        Image noise = GenImagePerlinNoise(
        size_.x, size_.y, position.x * size_.x, position.y * size_.y, scale_);
        texture_ = LoadTextureFromImage(noise);
        UnloadImage(noise);
    }

    chunk_t(const chunk_t&) = delete;

    chunk_t& operator=(const chunk_t&) = delete;

    friend void swap(chunk_t& a, chunk_t& b)
    {
        Texture2D tmp_texture = a.texture_;
        a.texture_            = b.texture_;
        b.texture_            = tmp_texture;

        chunk_position_t tmp_position = a.position_;
        a.position_                   = b.position_;
        b.position_                   = tmp_position;
    }

    chunk_t(chunk_t&& other) { swap(*this, other); }

    chunk_t& operator=(chunk_t&& other)
    {
        if(&other == this) {
            return *this;
        }
        swap(*this, other);
        return *this;
    }

    ~chunk_t()
    {
        if(texture_.id != 0) {
            UnloadTexture(texture_);
            texture_.id = 0;
        }
    }

    void render() const
    {
        DrawTextureRec(texture_, Rectangle { 0, 0, size_.x, size_.y },
        Vector2 { position_.x * size_.x, position_.y * size_.y }, WHITE);
    }

    chunk_position_t position() const { return position_; }

private:
    Texture2D texture_ { 0 };
    chunk_position_t position_ {};
};

class Map {
public:
    static constexpr integer max_chunks_ { 1024 };
    static constexpr integer loading_range_ { 4 };

    Map()
        : chunks_ { max_chunks_ }
    {
    }

    void render()
    {
        for(const auto& chunk : chunks_) {
            chunk.get_value().render();
        }
    }

    void update(chunk_position_t position)
    {
        for(integer i = -loading_range_; i <= loading_range_; ++i) {
            for(integer j = -loading_range_; j <= loading_range_; ++j) {
                chunk_position_t position_around { position.x + i, position.y + j };
                if(!chunks_.contains(position_around)) {
                    chunks_.put(position_around, chunk_t { position_around });
                }
            }
        }
    }

private:
    lru_map<chunk_position_t, chunk_t, chunk_position_t::hash, chunk_position_t::is_equal> chunks_;
};

int main(void)
{
    integer screen_width  = 800;
    integer screen_height = 450;
    InitWindow(screen_width, screen_height, "Dynamic loading and offloading");

    Camera2D camera;
    camera.zoom     = 100.0f / screen_height;
    camera.target   = { 0, 0 };
    camera.offset   = { screen_width / 2.0f, screen_height / 2.0f };
    camera.rotation = 0.0f;
    float speed     = 10.0f;

    Map map;

    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        if(IsKeyDown(KEY_RIGHT))
            camera.target.x += speed;
        if(IsKeyDown(KEY_LEFT))
            camera.target.x -= speed;
        if(IsKeyDown(KEY_DOWN))
            camera.target.y += speed;
        if(IsKeyDown(KEY_UP))
            camera.target.y -= speed;

        chunk_position_t position { 0, 0 };
        position.x = camera.target.x / chunk_t::size_.x;
        position.y = camera.target.y / chunk_t::size_.y;
        map.update(position);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        map.render();

        EndMode2D();

        DrawText(TextFormat("CURRENT FPS: %i", GetFPS()),
        GetScreenWidth() - 220, 40, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
