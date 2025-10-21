#pragma once
#include <raylib.h>

enum class HitSubject { Enemy, Player, Ufo, Base };

enum class HitOutcome { NonFatal, Destroyed };

struct Hit {
    static constexpr float DEFAULT_RADIUS = 2.5f;
    static constexpr int DEFAULT_TTL = 60;
    
    float x{0.0f};
    float y{0.0f};
    float radius{DEFAULT_RADIUS};
    int ttl{DEFAULT_TTL};
    HitSubject subject{HitSubject::Enemy};
    HitOutcome outcome{HitOutcome::NonFatal};
};
