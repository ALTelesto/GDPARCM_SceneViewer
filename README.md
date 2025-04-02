# GDPARCM_SceneViewer
 
Build instructions:
1. Add your local vcpkg include and lib directories inside VC++ Directories under project properties for gRPC includes and libraries.
2. In Main.cpp set IS_CLIENT to 0, build debug x64. Go to solution directory -> x64 -> Debug and rename both client.exe and client.pdb to server.exe and server.pdb.
3. Set IS_CLIENT to 1, build debug x64 again.
4. Go to solution directory -> GDPARCM_SceneViewer and copypaste 3D and shaders folders into the Debug path with your client and server exe files.

Controls:
- WASD to move
- Hold RMB to move camera
