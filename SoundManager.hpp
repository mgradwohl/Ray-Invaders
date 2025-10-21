#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <raylib.h>

namespace raylib {
    // Forward declaration
    class SharedSound;
    
    class SoundManager {
    public:
        // Get shared sound resource - creates if doesn't exist, increments reference count
        static std::shared_ptr<Sound> getSound(const std::string& filename);
        
        // Release sound resource - decrements reference count, unloads if count reaches zero
        static void releaseSound(const std::string& filename);
        
        // Get current reference count for a sound (for debugging)
        static int getReferenceCount(const std::string& filename);
        
        // Cleanup all sounds (called at shutdown)
        static void cleanup();
        
    private:
        struct SoundResource {
            Sound sound{};
            int refCount = 0;
            bool isValid = false;
        };
        
        static std::unordered_map<std::string, SoundResource> _soundCache;
        
        // Helper to load sound safely
        static Sound loadSoundSafe(const std::string& filename);
    };
    
    // RAII wrapper for shared sounds with automatic reference counting
    class SharedSound {
    public:
        SharedSound() = default;
        explicit SharedSound(const std::string& filename);
        ~SharedSound();
        
        // Copy constructor and assignment
        SharedSound(const SharedSound& other);
        SharedSound& operator=(const SharedSound& other);
        
        // Move constructor and assignment  
        SharedSound(SharedSound&& other) noexcept;
        SharedSound& operator=(SharedSound&& other) noexcept;
        
        // Sound operations
        [[nodiscard]] bool Play() const noexcept;
        void Stop() const noexcept;
        [[nodiscard]] bool IsPlaying() const noexcept;
        [[nodiscard]] bool IsValid() const noexcept;
        
        // Get filename for debugging
        [[nodiscard]] const std::string& getFilename() const noexcept { return _filename; }
        
    private:
        std::string _filename;
        std::shared_ptr<Sound> _sound;
        
        void acquire(const std::string& filename);
        void release();
    };
}