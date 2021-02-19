#!/bin/sh

cd components/audio_lib && git pull origin master && cd ../..
cd components/vgm_decoder && git pull origin master && cd ../..
cd components/sm_engine && git pull origin master && cd ../..
cd components/ram_logger && git pull origin main && cd ../..
cd components/spibus && git pull origin main && cd ../..
cd components/wire && git pull origin main && cd ../..

# git submodule foreach git pull origin master
