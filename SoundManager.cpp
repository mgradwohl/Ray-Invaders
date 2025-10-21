// Corresponding header
#include "SoundManager.hpp"

// Standard library headers
#include <iostream>

// Third-party headers
#include <raylib.h>

namespace raylib {
    
    // Static member definition
    std::unordered_map<std::string, SoundManager::SoundResource> SoundManager::_soundCache;
    
    std::shared_ptr<Sound> SoundManager::getSound(const std::string& filename) {
        auto& resource = _soundCache[filename];
        
        if (resource.refCount == 0) {
            // First time loading this sound
            resource.sound = loadSoundSafe(filename);
            resource.isValid = IsSoundValid(resource.sound);
            if (!resource.isValid) {
                std::cerr << "Warning: Failed to load sound: " << filename << std::endl;
            }
        }
        
        resource.refCount++;
        return std::shared_ptr<Sound>(&resource.sound, [filename](Sound*) {
            // Custom deleter that decrements reference count
            SoundManager::releaseSound(filename);
        });
    }
    
    void SoundManager::releaseSound(const std::string& filename)
    {
        auto it = _soundCache.find(filename);
        if (it != _soundCache.end()) {
            auto& resource = it->second;
            resource.refCount--;
            
            if (resource.refCount <= 0) {
                // No more references, unload the sound
                if (resource.isValid && IsSoundValid(resource.sound)) {
                    StopSound(resource.sound);
                    UnloadSound(resource.sound);
                }
                _soundCache.erase(it);
            }
        }
    }
    
    int SoundManager::getReferenceCount(const std::string& filename) {
        auto it = _soundCache.find(filename);
        return (it != _soundCache.end()) ? it->second.refCount : 0;
    }
    
    void SoundManager::cleanup()
    {
        for (auto& [filename, resource] : _soundCache) {
            if (resource.isValid && IsSoundValid(resource.sound)) {
                StopSound(resource.sound);
                UnloadSound(resource.sound);
            }
        }
        _soundCache.clear();
    }
    
    Sound SoundManager::loadSoundSafe(const std::string& filename) {
        Sound sound = LoadSound(filename.c_str());
        return sound;
    }
    
    // SharedSound implementation
    SharedSound::SharedSound(const std::string& filename) {
        acquire(filename);
    }
    
    SharedSound::~SharedSound() {
        release();
    }
    
    SharedSound::SharedSound(const SharedSound& other) {
        acquire(other._filename);
    }
    
    SharedSound& SharedSound::operator=(const SharedSound& other) {
        if (this != &other) {
            release();
            acquire(other._filename);
        }
        return *this;
    }
    
    SharedSound::SharedSound(SharedSound&& other) noexcept 
        : _filename(std::move(other._filename)), _sound(std::move(other._sound)) {
        other._filename.clear();
        other._sound.reset();
    }
    
    SharedSound& SharedSound::operator=(SharedSound&& other) noexcept {
        if (this != &other) {
            release();
            _filename = std::move(other._filename);
            _sound = std::move(other._sound);
            other._filename.clear();
            other._sound.reset();
        }
        return *this;
    }
    
    bool SharedSound::Play() const noexcept {
        if (_sound && IsValid()) {
            PlaySound(*_sound);
            return true;
        }
        return false;
    }
    
    void SharedSound::Stop() const noexcept {
        if (_sound && IsValid()) {
            StopSound(*_sound);
        }
    }
    
    bool SharedSound::IsPlaying() const noexcept {
        if (_sound && IsValid()) {
            return IsSoundPlaying(*_sound);
        }
        return false;
    }
    
    bool SharedSound::IsValid() const noexcept {
        return _sound && IsSoundValid(*_sound);
    }
    
    void SharedSound::acquire(const std::string& filename)
    {
        if (!filename.empty()) {
            _filename = filename;
            _sound = SoundManager::getSound(filename);
        }
    }
    
    void SharedSound::release()
    {
        if (!_filename.empty()) {
            // The shared_ptr destructor will automatically call SoundManager::releaseSound
            _sound.reset();
            _filename.clear();
        }
    }
    
} // namespace raylib