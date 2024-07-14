// Copyright 2024 Severin Denisenko

#include <cstdint>

#include <boost/container/list.hpp>
#include <boost/unordered/unordered_set.hpp>

#include <raylib.h>
#include <raymath.h>

using integer = int32_t;
struct chunk_position_t {
    integer x;
    integer y;

    friend bool operator==(const chunk_position_t& a, const chunk_position_t& b) {
        return a.x == b.x && a.y == b.y;
    }
};

std::size_t hash_value(const chunk_position_t& val) {
    boost::hash<integer> hasher;
    return hasher(val.x * val.y);
}

class chunk_t {
public:
    static constexpr Vector2 size_ { 64, 64 };

    chunk_t(chunk_position_t position)
        : position_(position) {
        Image noise = GenImagePerlinNoise(
        size_.x, size_.y, position.x * size_.x, position.y * size_.y, 1.0f);
        texture_ = LoadTextureFromImage(noise);
        UnloadImage(noise);
    }

    chunk_t(chunk_t&) = delete;
    chunk_t(chunk_t&& other) {
        texture_       = other.texture_;
        position_      = other.position_;
        other.texture_ = { 0 };
    }

    ~chunk_t() {
        if(texture_.id != 0) {
            UnloadTexture(texture_);
        }
    }

    void render() {
        DrawTextureRec(texture_, Rectangle { 0, 0, size_.x, size_.y },
        Vector2 { position_.x * size_.x, position_.y * size_.y }, WHITE);
    }

    chunk_position_t position() const {
        return position_;
    }

private:
    Texture2D texture_ { 0 };
    chunk_position_t position_ {};
};

class Map {
public:
    integer max_chunks_ { 128 };
    integer loading_range_ { 3 };

    void render() {
        for(chunk_t& chunk : chunks_) {
            chunk.render();
        }
    }

    void update(chunk_position_t position) {
        for(integer i = -loading_range_; i <= loading_range_; ++i) {
            for(integer j = -loading_range_; j <= loading_range_; ++j) {
                chunk_position_t position_around { position.x + i, position.y + j };

                if(!chunk_index_.contains(position_around)) {
                    chunk_index_.emplace(position_around);
                    chunks_.emplace_back(position_around);
                }
            }
        }

        while(chunks_.size() > max_chunks_) {
            chunk_index_.erase(chunks_.front().position());
            chunks_.pop_front();
        }
    }

private:
    using chunk_collection_t = boost::container::list<chunk_t>;
    using chunk_index_t = boost::unordered::unordered_set<chunk_position_t>;

    chunk_collection_t chunks_ {};
    chunk_index_t chunk_index_ {};
    integer last_chunk_ { 0 };
};

int main(void) {
    integer screen_width  = 800;
    integer screen_height = 450;
    InitWindow(screen_width, screen_height, "Dynamic loading and offloading");

    Camera2D camera;
    camera.zoom     = 200.0f / screen_height;
    camera.target   = { 0, 0 };
    camera.offset   = { screen_width / 2.0f, screen_height / 2.0f };
    camera.rotation = 0.0f;
    float speed     = 20.0f;

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
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        map.render();

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
