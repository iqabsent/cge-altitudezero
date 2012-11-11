# MScCGE-gp-2011 #
==============

Title: AltitudeZero

Fly .. shoot things .. don't get shot or crash.

By: Willem B. van der Merwe
33245888

!!Requires OpenAL!!


# Code layout: #

## assets.h ##

## collision.h ##

## gameobj.h ##
Happy with this.. or where this is going
Classes for game objects
### GameObj ###
Used as base for all game objects
Records vertices, x/y position, modelToWorld matrix & texture for rendering
Records friction, thrust, bounding box & radius, speed, etc. for "physics"
Has virtual function for boundary checks
Has a function for simulating physics & some game mechanics
### Projectile ###
Used for bullets
Records it's own damage
Has its own boundary check function
### Craft ###
Used as base for enemy and player craft
Records its own health
Has functions for adjusting health (heal and damage)
### Fighter ###
Has own boundary check and simulate function
### Enemy ###
Has own boundary check
Has function for behaviour .. which does nothing

## render.h ##
Sets up compiles and links a shader program
Has a function to render RenderData

## structs.h ##
Contains two structs RenderData and CollisionData used in game

## Borrowed from Andy: ##
file_manager.h
matrix.h
sound_manager.h
texture_manager.h
vector.h


# Creds: #
arboris.deviantart.com for the Fighter space-ship sprites
http://arboris.deviantart.com/art/Spaceship-sprites-43030167

Skorpio for the Enemy space-ship sprites
http://opengameart.org/content/top-down-space-fighters

Quake3 audio team for their machinegun and rocket sounds

Andy Thomason for several of the .h classes which made this
"game" possible within a week of coding

ENOISE ( http://e-noise.com/ ) for being flexible on work hours