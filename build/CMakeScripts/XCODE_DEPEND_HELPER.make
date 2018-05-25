# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.particleMan.Debug:
PostBuild.glfw.Debug: /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Debug/particleMan
/Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Debug/particleMan:\
	/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Debug/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Debug/particleMan


PostBuild.glfw.Debug:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Debug/libglfw3.a:
	/bin/rm -f /Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Debug/libglfw3.a


PostBuild.particleMan.Release:
PostBuild.glfw.Release: /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Release/particleMan
/Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Release/particleMan:\
	/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Release/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/Release/particleMan


PostBuild.glfw.Release:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Release/libglfw3.a:
	/bin/rm -f /Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Release/libglfw3.a


PostBuild.particleMan.MinSizeRel:
PostBuild.glfw.MinSizeRel: /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/MinSizeRel/particleMan
/Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/MinSizeRel/particleMan:\
	/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/MinSizeRel/particleMan


PostBuild.glfw.MinSizeRel:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a:
	/bin/rm -f /Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a


PostBuild.particleMan.RelWithDebInfo:
PostBuild.glfw.RelWithDebInfo: /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/RelWithDebInfo/particleMan
/Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/RelWithDebInfo/particleMan:\
	/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a\
	/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a
	/bin/rm -f /Users/sydney/Documents/CSC474/finalprog\ -\ ParticleMan/build/RelWithDebInfo/particleMan


PostBuild.glfw.RelWithDebInfo:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a:
	/bin/rm -f /Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a




# For each target create a dummy ruleso the target does not have to exist
/Applications/Autodesk/FBX\ SDK/2019.0/lib/clang/release/libfbxsdk.a:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Debug/libglfw3.a:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a:
/Users/sydney/Documents/Collections/Software\ Downloads/glfw-3.2.1/debug/src/Release/libglfw3.a:
