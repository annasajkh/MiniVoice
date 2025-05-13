# MiniVoice
Record voice from microphone and play it back<br> 
it's using miniaudio audio library

# Build on Windows

```markdown
mkdir build_windows && cd build_windows
cmake ..
msbuild MiniVoiceTest/MiniVoiceTest.sln /p:Configuration=Release
```

# Build on Linux

```markdown
mkdir build_linux && cd build_linux
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

# Using
minimal example is in MiniVoiceTest 