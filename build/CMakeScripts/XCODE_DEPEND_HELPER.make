# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.particleMan.Debug:
PostBuild.glfw.Debug: /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Debug/particleMan
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Debug/particleMan:\
	/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Debug/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Debug/particleMan


PostBuild.glfw.Debug:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Debug/libglfw3.a:
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Debug/libglfw3.a


PostBuild.particleMan.Release:
PostBuild.glfw.Release: /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Release/particleMan
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Release/particleMan:\
	/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Release/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/Release/particleMan


PostBuild.glfw.Release:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Release/libglfw3.a:
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Release/libglfw3.a


PostBuild.particleMan.MinSizeRel:
PostBuild.glfw.MinSizeRel: /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/MinSizeRel/particleMan
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/MinSizeRel/particleMan:\
	/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/MinSizeRel/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/MinSizeRel/particleMan


PostBuild.glfw.MinSizeRel:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/MinSizeRel/libglfw3.a:
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/MinSizeRel/libglfw3.a


PostBuild.particleMan.RelWithDebInfo:
PostBuild.glfw.RelWithDebInfo: /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/RelWithDebInfo/particleMan
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/RelWithDebInfo/particleMan:\
	/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/RelWithDebInfo/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/RelWithDebInfo/particleMan


PostBuild.glfw.RelWithDebInfo:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/RelWithDebInfo/libglfw3.a:
	/bin/rm -f /Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/RelWithDebInfo/libglfw3.a




# For each target create a dummy ruleso the target does not have to exist
/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Debug/libglfw3.a:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/MinSizeRel/libglfw3.a:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/RelWithDebInfo/libglfw3.a:
/Users/noahpaige/Desktop/csc/csc474/finalproject/ParticleMan/build/ext/glfw-3.3-prerelease/debug/src/Release/libglfw3.a:
