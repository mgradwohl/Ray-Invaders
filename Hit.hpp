#pragma once
#include <raylib.h>

enum class HitSubject { Enemy, Player, Ufo, Base };

enum class HitOutcome { NonFatal, Destroyed };

struct Hit {
    float x{0.0f};
    float y{0.0f};
    float radius{2.5f};
    int ttl{60};
    HitSubject subject{HitSubject::Enemy};
    HitOutcome outcome{HitOutcome::NonFatal};
};
