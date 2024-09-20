#!/bin/sh
cd $SRCROOT/shaders
/usr/local/bin/glslangValidator -V shader.vert
/usr/local/bin/glslangValidator -V shader.frag
mkdir $TARGET_BUILD_DIR/$CONTENTS_FOLDER_PATH/shaders
mv vert.spv $TARGET_BUILD_DIR/$CONTENTS_FOLDER_PATH/shaders/vert.spv
mv frag.spv $TARGET_BUILD_DIR/$CONTENTS_FOLDER_PATH/shaders/frag.spv

